from flask import Flask, render_template, json, redirect
from master.Daemon import Daemon
import copy

app = Flask(__name__)


## CONSTANTS ##

constants = {
    'maxStacks' : 16,
    'nSlavesByStack' : 6,
    'stackHeatLimit' : 75,
    'raspStatus' : [
        'Off',
        'On'
    ]
}


## DATA ##

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

def getStack(id=None):
    master = daemon.get_master()

    def renderStack(stack):

        rasps = stack.get_pi_devices()

        stackJSON = {
            'heat': 80,
            'rasps': {}
        }

        for rasp in stack.get_pi_devices():
            stackJSON['rasps'][rasp.get_pos()] = rasp.get_i2c_address()

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
            'address' : rasp.get_i2c_address(),
            'stack' : 1,
            'os' : rasp.get_os(),
            'status' : 1,
            'ip' : rasp.get_ip_address(),
            'cpu' : rasp.get_cpu_usage(),
            'ram' : rasp.get_ram_usage()
        }

    if id is not None:
        rasp = master.get_slave_by_id(id)
        if rasp is not None:
            return renderRasp(rasp)
        else:
            return None
    else:
        rasps = {}
        for stack in master.get_stacks():
            for rasp in stack.get_pi_devices():
                rasps[rasp.get_i2c_address()] = renderRasp(rasp)
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
        return routeDefault()

@app.route('/view/rasp/<int:id>') # = route options
def viewRasp(id):
    if getRasp(id) is not None:
        return render_template('rasp.html', constants=constants, raspId=id)
    else:
        return routeDefault()
        

        
#######################################################################################
#                                                                                     #
#                                   REST ROUTES                                       #
#                                                                                     #
#######################################################################################

# STACK GET

@app.route("/stack/", defaults={'id':None}, methods=['GET'])
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

    return app.response_class(
        response=json.dumps(stack),
        status=200,
        mimetype='application/json')

# RASP GET

@app.route("/rasp/", defaults={'id':None}, methods=['GET'])
@app.route("/rasp/<int:id>")
def routeRasp(id):
    rasp = getRasp(id)
    if rasp is None:
        rasp = {}
    return app.response_class(
        response=json.dumps(rasp),
        status=200,
        mimetype='application/json')

# STACK ACTIONS

@app.route("/stack/shutdown", defaults={'id':None}, methods=['POST'])
@app.route('/stack/shutdown/<int:id>')
def routeShutdown(id):
    stack = daemon.get_master().get_stack(id)

    if stack is None:
        response = 0
    else
        response = 1
    
    return app.response_class(
        response=json.dumps({'response': response}),
        status=200,
        mimetype='application/json')
        
# RASP ACTIONS

@app.route("/rasp/start", defaults={'id':None}, methods=['POST'])
@app.route('/rasp/start/<int:id>')
def routeStart(id):
    rasp = daemon.get_master().get_slave_by_id(id)

    if rasp is None:
        response = 0
    else
        response = 1

    return app.response_class(
        response=json.dumps({'response': response}),
        status=200,
        mimetype='application/json')

@app.route("/rasp/stop", defaults={'id':None}, methods=['POST'])        
@app.route('/rasp/stop/<int:id>')
def routeStop(id):
    rasp = daemon.get_master().get_slave_by_id(id)

    if rasp is None:
        response = 0
    else
        response = 1
    
    return app.response_class(
        response=json.dumps({'response': response}),
        status=200,
        mimetype='application/json')

@app.route("/rasp/restart", defaults={'id':None}, methods=['POST'])        
@app.route('/rasp/restart/<int:id>')
def routeRestart(id):
    rasp = daemon.get_master().get_slave_by_id(id)

    if rasp is None:
        response = 0
    else
        response = 1
    
    return app.response_class(
        response=json.dumps({'response': response}),
        status=200,
        mimetype='application/json')
        
        
## RUN ##

if __name__ == '__main__':
    daemon = Daemon()
    daemon.start()
    app.run(debug=True)
