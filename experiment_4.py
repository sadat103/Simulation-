import heapq
import random
import matplotlib.pyplot as plt
from lcgrand import MODLUS
from lcgrand import MULT1
from lcgrand import MULT2
from lcgrand import zrng
from lcgrand import lcgrand
import math

IDL = 0
BUSY = 1

# Parameters
class Params:
    def __init__(self, lambd, mu, k, q):
        self.lambd = lambd  # interarrival rate
        self.mu = mu  # service rate
        self.k = k
        self.queueNo = q
    # Note lambd and mu are not mean value, they are rates i.e. (1/mean)

# Write more functions if required


# States and statistical counters
class States:
    def __init__(self):
        # States
        self.queue = [] #this queue stores the arrival times
        self.status = [] #this holds the status of k servers
        self.numInQ = 0.0
        self.timeLastEvent = 0.0
        self.util = 0.0
        self.avgQdelay = 0.0
        self.avgQlength = 0.0
        self.served = 0
        self.totDely = 0.0
        self.arNmQueue = 0.0
        self.arServStatus = 0.0



    def update(self, sim, event):
        #if the event is the START event
        if event.eventType == 'START':
            return

        tmSincLasEvnt = event.eventTime - self.timeLastEvent
        self.timeLastEvent = event.eventTime
        #update area under numberInQ curve
        r = self.numInQ * tmSincLasEvnt
        self.arNmQueue = self.arNmQueue + r

        #update area under server-busy indicator function
        buServrs = 0
        for i in range(sim.params.k):
            if self.status[i]==BUSY:
                buServrs = buServrs + 1
        w = buServrs/sim.params.k
        c = w * tmSincLasEvnt
        self.arServStatus = self.arServStatus + c
        

    def finish(self, sim):
        self.avgQdelay = self.totDely/self.served
        b = self.arNmQueue/sim.simclock
        self.avgQlength = b/sim.params.queueNo
        self.util = self.arServStatus/sim.simclock

    def printResults(self, sim):
        
        print("\n\nResults from experiment.")
        # DO NOT CHANGE THESE LINES
        print('MMk Results: lambda = %lf, mu = %lf, k = %d' % (sim.params.lambd, sim.params.mu, sim.params.k))
        print('MMk Total customer served: %d' % (self.served))
        print('MMk Average queue length: %lf' % (self.avgQlength))
        print('MMk Average customer delay in queue: %lf' % (self.avgQdelay))
        print('MMk Time-average server utility: %lf' % (self.util))

    def getResults(self, sim):
        return (self.avgQlength, self.avgQdelay, self.util)

    def initializeStat(self, k:int):
        self.status = []
        i=0
        while i<k:
            self.status.append(IDL)
            i=i+1

    def checkingStat(self, k):
        i=0
        while i<k:
            if self.status[i]==IDL:
                return i
            i=i+1
        return -1

    def initializeQu(self, k:int):
        self.queue = []
        i=0
        while i<k:
            self.queue.append([])

            i+=1
            

class Event:
    def __init__(self, sim):
        self.eventType = None
        self.sim = sim
        self.eventTime = None

    def process(self, sim):
        raise Exception('Unimplemented process method for the event!')

    def __repr__(self):
        return self.eventType


class StartEvent(Event):
    def __init__(self, eventTime, sim):
        self.eventTime = eventTime
        self.eventType = 'START'
        self.sim = sim

    def process(self, sim):
        # this is the startEvent. It will enqueue an arrival event
        p = sim.params.lambd
        q = 1/p
        firstArrivalTime = self.eventTime + sim.exponential(q)
        sim.scheduleEvent(ArrivalEvent(firstArrivalTime, sim))
        sim.scheduleEvent(ExitEvent(10000, sim))

class ExitEvent(Event):
    def __init__(self, eventTime, sim):
        self.eventTime = eventTime
        self.eventType = 'EXIT'
        self.sim = sim

    def process(self, sim):
        None


class ArrivalEvent(Event):
    # Write __init__ function
    def __init__(self, eventTime ,sim):
        self.eventTime = eventTime
        self.sim = sim
        self.eventType = 'ARRIVAL'

    def process(self, sim):
        #schedule the next arrival
        p = 1/sim.params.lambd
        t = sim.exponential(p)
        nxtArvl = sim.simclock + t
        sim.scheduleEvent(ArrivalEvent(nxtArvl, sim))

        #check to see if any server is IDL
        NumOfFrServ = sim.states.checkingStat(self.sim.params.k)
        if NumOfFrServ == -1:
            sim.states.numInQ = sim.states.numInQ + 1
            #if all the servers are busy, then put the event in the leftmost shortest queue
            shtsQueue = 0
            index = 1
            f = sim.params.queueNo
            while index<f:
                a = sim.states.queue[index]
                b = sim.states.queue[shtsQueue]
                if len(a)<len(b):
                    shtsQueue = index

                index = index + 1
            sim.states.queue[shtsQueue].append(sim.simclock) #this is the arrival time
        else:
            dl = 0.0
            sim.states.totDely = sim.states.totDely + dl
            #increment the number of customers served and make server busy
            sim.states.served += 1
            sim.states.status[NumOfFrServ] = BUSY

            #create the departure event for this arrival
            p = 1/sim.params.mu
            ot = sim.exponential(p)
            deprtTim = sim.simclock + ot
            sim.scheduleEvent(DepartureEvent(deprtTim, sim, serverNo=NumOfFrServ))


class DepartureEvent(Event):
    
    def __init__(self, eventTime, sim, serverNo:int):
        self.eventTime = eventTime
        self.eventType = 'DEPART'
        self.sim = sim
        self.serverNo = serverNo


    def process(self, sim):
        x = sim.states.queue
        y = sim.states.queue[self.serverNo]
        z = sim.states.queue[0]
        #if there is no one in the particular queue from which the departure event is being issued, make the server IDL
        if len(x)>1 and len(y)==0:
            
            sim.states.status[self.serverNo] = IDL


        elif len(x)==1 and len(z)==0:
            
            sim.states.status[self.serverNo] = IDL


        else:
            #queue of this particular server is nonempty, so let the first person in queue receive service
            sim.states.numInQ -= 1

            #the we get the arrival time of the first person
            #in the queue(sim.states.queue) and calculate the delay faced
            if sim.params.queueNo>1:
                u = sim.states.queue[self.serverNo][0]
                delay = sim.simclock - u
            else:
                p = sim.states.queue[0][0]
                delay = sim.simclock - p
            sim.states.totDely += delay
            #increment the number of customers served and schedule departure
            sim.states.served += 1
            p = 1/sim.params.mu
            t= sim.exponential(p)
            deprtTim = sim.simclock + t
            sim.scheduleEvent(DepartureEvent(deprtTim, sim, self.serverNo))

            #move everyone in the queue one step up
            if sim.params.queueNo>1:
                sim.states.queue[self.serverNo] = sim.states.queue[self.serverNo][1:]
            else:
                sim.states.queue[0] = sim.states.queue[0][1:]
            #when there are k queues,
            #I'll check whether any person from the queue on the left or from the queue on the right can join this queue
            if len(sim.states.queue)>1:

                bigQueue = -1
                highestDifference = -1
                if self.serverNo==0:
                    bigQueue = self.serverNo + 1
                    a = sim.states.queue[bigQueue]
                    b = sim.states.queue[self.serverNo]
                    highestDifference = len(a) - len(b)
                elif self.serverNo==(sim.params.k-1):
                    bigQueue = self.serverNo - 1
                    m = sim.states.queue[bigQueue]
                    n = sim.states.queue[self.serverNo]
                    highestDifference = len(m) - len(n)
                else:
                    f = sim.states.queue[self.serverNo+1] 
                    g = sim.states.queue[self.serverNo-1] 
                    h = sim.states.queue[self.serverNo]
                    rtQueue = len(f) - len(h)
                    ltQueue = len(g) - len(h)

                    if rtQueue>ltQueue:
                        highestDifference = rtQueue
                        bigQueue = self.serverNo + 1
                    else:
                        highestDifference = ltQueue
                        bigQueue = self.serverNo - 1
                #if there is any queue(either on left or right) which has at least two more people, then one people from the end of the queue will join this queue
                if highestDifference>=2:
                    sim.states.queue[self.serverNo].append(sim.states.queue[bigQueue][-1])
                    #adjust the queue from which the person left
                    sim.states.queue[bigQueue] = sim.states.queue[bigQueue][0:-1]


class Simulator:
    def __init__(self, seed):
        self.eventQ = [] #this stores the arrival and departure events
        self.simclock = 0
        self.seed = seed
        self.params = None
        self.states = None

    def initialize(self):
        self.simclock = 0
        self.states.initializeStat(self.params.k)
        self.states.initializeQu(self.params.queueNo)
        self.scheduleEvent(StartEvent(0, self))

    def configure(self, params, states):
        self.params = params
        self.states = states

    def now(self):
        return self.simclock

    def scheduleEvent(self, event):
        #heapq is a priority queue
        heapq.heappush(self.eventQ, (event.eventTime, event))

    def run(self):
        random.seed(self.seed)
        self.initialize()

        while len(self.eventQ) > 0:
            time, event = heapq.heappop(self.eventQ)

            if event.eventType == 'EXIT':
                # event.process(self)
                break

            if self.states != None:
                self.states.update(self, event)

            self.simclock = event.eventTime
            event.process(self)
            

        self.states.finish(self)
        
    def exponential(self, mean):
        c = math.log(lcgrand(1))
        return (-mean * c)
    
    def printResults(self):
        self.states.printResults(self)

    def getResults(self):
        return self.states.getResults(self)


def experiment4():
    seed = 110
    avglength = []
    avgdelay = []
    util = []
    numOfK = [1,2,3,4]

    for k in numOfK:
        print(f"iteration {k}")
        zrng[1] = 1973272912
        sim = Simulator(seed)
        sim.configure(Params(5.0/60, 8.0/60, k, k), States())
        sim.run()
        sim.printResults()

        length, delay, utl = sim.getResults()
        avglength.append(length)
        avgdelay.append(delay)
        util.append(utl)
        print("\n\n")

    plt.figure(1)
    plt.subplot(311)
    plt.plot(numOfK, avglength)
    plt.xlabel(' K ')
    plt.ylabel('Avg Q length')

    plt.subplot(312)
    plt.plot(numOfK, avgdelay)
    plt.xlabel(' K ')
    plt.ylabel('Avg Q delay (sec)')

    plt.subplot(313)
    plt.plot(numOfK, util)
    plt.xlabel(' K ')
    plt.ylabel('Util')

    plt.show()


def main():
    print("\n\nExperiment 4")
    experiment4()


if __name__ == "__main__":
    main()
