import sys
from flask import Flask, render_template, json, redirect
from master.Daemon import Daemon
import copy

app = Flask(__name__)


## CONSTANTS ##

constants = {
    'maxStacks' : 16,
    'nSlavesByStack' : 6,
    'stackHeatLimit' : 75,
    'raspCPULimit' : 80,
    'status' : [
        'Off',
        'On'
    ]
}


## DATA ##
"""
stacksTest = {
    1 : {
        'heat' : 80,
        'rasps' : {
            0 : 1,
            2 : 2
        }
    },
    2 : {
        'heat' : 25,
        'rasps' : {
            3 : 42
        }
    }
}

raspsTest = {
    1 : {
        'name' : 'Cran',
        'address' : 1,
        'stack' : 1,
        #'heat' : '70',
        'os' : 'Pidora',
        'status' : 1,
        'ip' : "192.168.0.1",
        'cpu' : 42
    },
    2 : {
        'name' : 'Straw',
        'address' : 2,
        'stack' : 1,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 0,
        'ip' : "192.168.0.2",
        "cpu" : 32
    },
    42 : {
        'name' : 'Blue',
        'address' : 42,
        'stack' : 2,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 1,
        'ip' : "192.168.0.42",
        'cpu' : 74
    }
}
"""

## GETTERS ##
"""
def getStack(id=None):
    if id is not None:
        if stacksTest.get(id) is not None:
            return copy.deepcopy(stacksTest[id])
        else:
            return None
    else:
        return copy.deepcopy(stacksTest)

def getRasp(id=None):
    if id is not None:
        if raspsTest.get(id) is not None:
            return copy.deepcopy(raspsTest[id])
        else:
            return None

    else:
        return copy.deepcopy(raspsTest)

"""

def json_response(response):
    return app.response_class(
        response=json.dumps({'response': response}),
        status=200,
        mimetype='application/json')

def getStack(id=None):
    master = daemon.get_master()

    def renderStack(stack):

        rasps = stack.get_pi_devices()

        stackJSON = {
            'heat': 80,
            'status': 1,
            'rasps': {}
        }

        for rasp in stack.get_pi_devices():
            stackJSON['rasps'][rasp.get_pos()] = rasp.get_i2c()

        return stackJSON

    if id is not None:
        stack = master.get_stack(id)
        if stack is not None:
            return renderStack(stack)
        else:
            return None
    else:
        stacks = {}
        counter = 1
        for stack in master.get_stacks():
            stacks[counter] = renderStack(stack)
            counter += 1
        return stacks

def getRasp(id=None):
    master = daemon.get_master()

    def renderRasp(rasp):
        return {
            'name' : 'Name',
            'address' : rasp.get_i2c(),
            'stack' : 1,
            'os' : rasp.get_os(),
            'status' : 1,
            'ip' : rasp.get_ip_address(),
            'cpu' : rasp.get_cpu_usage(),
            'ram' : rasp.get_ram_usage()
        }

    if id is not None:
        rasp = master.get_slave_by_i2c(id)
        print(id)
        print(type(id))
        print(rasp)
        if rasp is not None:
            return renderRasp(rasp)
        else:
            return None
    else:
        rasps = {}
        for stack in master.get_stacks():
            for rasp in stack.get_pi_devices():
                rasps[rasp.get_i2c()] = renderRasp(rasp)
        return rasps


#######################################################################################
#                                                                                     #
#                                    VIEWS                                            #
#                                                                                     #
#######################################################################################

@app.route("/index")
@app.route("/")
@app.route("/view/stack")
@app.route("/view/rasp")
def routeDefault():
    return redirect("/view", code=302)

@app.route("/view")
def viewDefault():
    return render_template('index.html', constants=constants)

@app.route('/view/stack/<int:id>') # = route options
def viewStack(id):
    if getStack(id) is not None:
       return render_template('stack.html', constants=constants, stackId=id)
    else:
        return routeDefault()#would be better to say there's an error... To-Do

@app.route('/view/rasp/<int:id>') # = route options
def viewRasp(id):
    if getRasp(id) is not None:
        return render_template('rasp.html', constants=constants, raspId=id)
    else:
        return routeDefault()#same here
        

        
#######################################################################################
#                                                                                     #
#                                   REST ROUTES                                       #
#                                                                                     #
#######################################################################################

# STACK GET

@app.route("/stack", defaults={'id':None})
@app.route("/stack/<int:id>")
def routeStack(id):
    def nestRaspsInStack(stack):
        for rasp in stack['rasps']:
            stack['rasps'][rasp] = getRasp(stack['rasps'][rasp])

    stack = getStack(id)
    if stack is None:
        stack = {}
    else:
        if id is None:
            for stackId in stack:
                nestRaspsInStack(stack[stackId])
        else:
            nestRaspsInStack(stack)

    return json_response(json.dumps(stack))

# STACK ACTIONS

@app.route('/stack/<int:id>/start', methods=['POST'])
def stackStart(id):
    stack = daemon.get_master().get_stack(id).enable_alimentation()

    if stack is None:
        response = 0
    else:
        response = 1
    
    return json_response(json.dumps({'response': response}))

@app.route('/stack/<int:id>/shutdown', methods=['POST'])
def stackShutdown(id):
    stack = daemon.get_master().get_stack(id).disable_alimentation()

    if stack is None:
        response = 0
    else:
        response = 1
    
    return json_response(json.dumps({'response': response}))

# RASP GET

@app.route("/rasp/", defaults={'id':None})
@app.route("/rasp/<int:id>")
def routeRasp(id):
    rasp = getRasp(id)
    if rasp is None:
        rasp = {}
    return json_response(json.dumps(rasp))
        
# RASP ACTIONS

@app.route('/rasp/<int:id>/start', methods=['POST'])
def raspStart(id):
    rasp = daemon.get_master().get_slave_by_i2c(id)

    if rasp is None:
        response = 0
    else:
        response = 1

    return json_response(json.dumps({'response': response}))
    
@app.route('/rasp/<int:id>/stop', methods=['POST'])
def raspStop(id):
    rasp = daemon.get_master().get_slave_by_i2c(id)

    if rasp is None:
        response = 0
    else:
        response = 1
    
    return json_response(json.dumps({'response': response}))
     
@app.route('/rasp/<int:id>/restart', methods=['POST'])
def raspRestart(id):
    rasp = daemon.get_master().get_slave_by_i2c(id)

    if rasp is None:
        response = 0
    else:
        response = 1
    
    return json_response(json.dumps({'response': response}))
        
        
## RUN ##

if __name__ == '__main__':
    ip = "127.0.0.2"
    if (len(sys.argv) > 1):
        ip = sys.argv[1]
    daemon = Daemon(ip)
    daemon.start()
    app.run(debug=True)
