"""
The Boggart worker API
"""
import zmq
import json
import time
from threading import Thread

try:
    from Queue import Queue
except ImportError:
    import queue.Queue as Queue


class BoggartWorker(object):
    """
    Boggart worker inspired by the http://rfc.zeromq.org/spec:7
    """
    def __init__(self, service, host=None, port=None, numworkers=1):
        self.service = service
        # TODO validate the host and port formats here
        if host and port:
            self.endpoint_url = "{host}:{port}".format(host=host, port=port)
        else:
            self.endpoint_url = "tcp://localhost:5777"

        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.DEALER)
        self.socket.connect(self.endpoint_url)
        self.registered_tasks = list()
        self.num_workers = numworkers
        self.worker_pool = self.ThreadPool(self.num_workers, self.context)
        # Setup internal communication plumbing
        self.internal_endpoint_url = "inproc://boggartworker"
        self.inter_socket = self.context.socket(zmq.ROUTER)
        self.inter_socket.bind(self.internal_endpoint_url)


    def _send_to_broker(self, msg_parts):
        """
        Submits the message part to the broker. The msg_parts are assumed to be
        in correct order as per the spec hence no validation or reformatting is carried out here
        :params msg_parts: an iterable containing the message parts in the correct order
        """
        self.socket.send_multipart(msg_parts)

    def register_task(self, func):
        """
        Adds the func as a task to be executed by the worker pool. Everytime a job is available from
        the queue this func will be called with the payload from the queue
        Only one task can be registered
        param func: the function name
        raises ValueError if there's already a registered task
        """
        if len(self.registered_tasks) == 1:
            raise ValueError("Cannot register task {0}. Task {1} already registered"\
                    .format(func, self.registered_tasks[0]))

        self.registered_tasks.append(func)

    def run(self):
        """
        Starts and manages the worker process
        """
        # send ready command on startup
        ready = ['', 'BOGW01'.encode(), "READY", self.service, str(self.num_workers)]
        self._send_to_broker(ready)
        poller = zmq.Poller()
        poller.register(self.socket, zmq.POLLIN)
        poller.register(self.inter_socket, zmq.POLLIN)
        while True:
            print "waiting my address is ", self.socket.identity
            res = poller.poll()
            print "Got something"
            if self.socket in dict(res).keys():
                resp = self.socket.recv_multipart()
                print resp
                job_id = resp[0]
                job_payload = resp[1]

                self.worker_pool.add_task(self.registered_tasks[0], job_payload, bog_job_id=job_id)

                
            elif self.inter_socket in dict(res).keys():
                resp = self.inter_socket.recv_multipart()
                job_id = resp[1]
                job_results = resp[2]
                print "Sending back ",resp
                broker_resp = ['', 'BOGW01'.encode(), "WORKER_RESP",\
                                self.service, job_id, job_results]

                self.socket.send_multipart(broker_resp)
            else:
                print "Unknown socket "+dict(res)

    class ThreadPool(object):
        """
        Responsible for managing the worker pool
        """

        def __init__(self, numworkers, context):
            self.tasks = Queue(numworkers)

            for _ in range(numworkers):
                self.Worker(self.tasks, context)

        def add_task(self, func, *args, **kwargs):
            """
            Adds a task to be executed by the worker pool.
            This function will block until there is a slot in the queue
            param func: the function name
            param args: positional args to be passed to func when being executed
            param kwargs: keyword args to be passed to func when being executed
            """
            self.tasks.put((func, args, kwargs))

        def wait_completion(self):
            """
            Wait until all the tasks in the queue have finished
            """
            self.tasks.join()

        class Worker(Thread):
            """
            The guy who does all the work
            """
            def __init__(self, tasks, context):
                Thread.__init__(self)
                self.tasks = tasks
                self.daemon = True
                self.start()

                # setup internal communication plumbing
                self.context = context
                self.internal_endpoint_url = "inproc://boggartworker"
                self.worker_socket = self.context.socket(zmq.DEALER)
                self.worker_socket.connect(self.internal_endpoint_url)

            def run(self):
                while True:
                    func, args, kwargs = self.tasks.get()
                    print args, kwargs
                    job_id = kwargs['bog_job_id']
                    del kwargs['bog_job_id']
                    error = False
                    try:
                        res = str(func(*args, **kwargs))
                    except Exception, e:
                        error = True
                        err_str = str(e)
                    finally:
                        if error:
                            res = err_str
                        payload = [job_id, res]
                        self.worker_socket.send_multipart(payload)
                        self.tasks.task_done()

def testing(slep):
    print "I was called doing the work "+str(slep)
    return "These are the results "

if __name__ == "__main__":
    worker = BoggartWorker("test", numworkers=5)
    worker.register_task(testing)
    worker.run()