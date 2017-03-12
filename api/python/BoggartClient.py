import zmq
import json 
import time 

MAX_TRIES = 5 # number of times we are going to retry before giving up
MAX_IO_WORKERS = 10 # number of threads that we are going to spawn
QUEUE_ENDPOINT = "tcp://localhost:5777"
TIMEOUT = 3000

class BoggartClientException(Exception):
    """
    Generic Exception class for the boggart client
    """
    pass


class BoggartClient(object):
    """
    This is the worker process that handles the pushing out of the job to the BROKER
    as per the spec:
    This is a client implemention of http://rfc.zeromq.org/spec:7
    """

    def __init__(self, host=None, port=None):
        self.context = zmq.Context()
        self.signature = 'BOGC01'.encode()
        if host and port:
            # TODO validate host and port formats here
            self.endpoint = "{host}:{port}".format(host=host, port=port)
        else:
            self.endpoint = QUEUE_ENDPOINT

    def push_job(self, job, service, block=False):
        """
        Pushes the job to the queue using the QUEUE_ENDPOINT
        :param job: json string with the job details. N/B: No verification of \
        the json obj is carried out
        :param: service: str representing the name of the service/queue this job should be pushed to
        :param block: bool for whether to push asynchronously or synchronously
        return result: <json>
        raise BoggartWorkerException if no response is received within the timeout period
        """

        cmd = "INSJOB" # insert job command
        payload = [self.signature, cmd, service, job, str(block)]

        if block:
            client_sock = self.context.socket(zmq.REQ)
        else:
            client_sock = self.context.socket(zmq.DEALER)
            # when using the dealer socket we must add the empty frame manually as
            # the first part of the payload
            payload.insert(0, '')

        client_sock.connect(self.endpoint)
        client_sock.send_multipart(payload)
        poller = zmq.Poller()
        poller.register(client_sock)

        while True:
            res = poller.poll(timeout=TIMEOUT)
            if client_sock in dict(res).keys():
                resp = client_sock.recv_multipart()
                return resp
            else:
                poller.unregister(client_sock)
                raise BoggartClientException("Could not get response from the queue within\
                {} sec".format(TIMEOUT/1000))

    def get_job_results(self, jobid):
        """
        Get the job (identified by the jobid )results from the broker
        """
        client_sock = self.context.socket(zmq.REQ)
        cmd = "GETJOB" # get job results command
        payload = [self.signature, cmd, jobid]
        client_sock.connect(self.endpoint)
        poller = zmq.Poller()
        poller.register(client_sock)
        client_sock.send_multipart(payload)
        res = poller.poll(timeout=TIMEOUT)
        if client_sock in dict(res).keys():
            resp = client_sock.recv_multipart()
            return resp

if __name__ == "__main__":
    client = BoggartClient()
    d = json.dumps({'name': 'mike', 'age': '25'})
    res = client.push_job(d, "test", False)
    time.sleep(4)
    res = client.get_job_results(res[0])
    print (res)