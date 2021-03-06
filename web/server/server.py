DEBUG = False

import sys
from flask import Flask, render_template, json, redirect
if not DEBUG:
    from master.Daemon import Daemon
import copy

app = Flask(__name__)


## CONSTANTS ##

constants = {
    'maxStacks' : 16,
    'nSlavesByStack' : 6,
    'stackHeatLimit' : 75,
    'raspCPULimit' : 80,
    'raspRAMLimit' : 80
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
        'os' : 'Pidora',
        'ip' : "192.168.0.1",
        'cpu' : 42,
        'ram' : 80
    },
    2 : {
        'name' : 'Straw',
        'address' : 2,
        'stack' : 1,
        'os' : 'Raspbian',
        'ip' : "192.168.0.2",
        "cpu" : 32,
        'ram' : 30
    },
    42 : {
        'name' : 'Blue',
        'address' : 42,
        'stack' : 2,
        'os' : 'Raspbian',
        'ip' : "192.168.0.42",
        'cpu' : 74,
        'ram' : 30
    }
}


## GETTERS ##

def getStackObject(id=None):
    if id is not None:
        if DEBUG:
            return copy.deepcopy(stacksTest[id])
        else:
            return daemon.get_master().get_stack(id - 1)
    else:
        if DEBUG:
            return copy.deepcopy(stacksTest)
        else:
            stacks = {}
            counter = 1
            for stack in daemon.get_master().get_stacks():
                stacks[counter] = stack
                counter += 1
            return stacks

def getRaspObject(id=None):
    if id is not None:
        if DEBUG:
            return copy.deepcopy(raspsTest[id])
        else:
            return daemon.get_master().get_slave_by_i2c(id)
    else:
        if DEBUG:
            return copy.deepcopy(raspsTest)
        else:
            rasps = {}
            for stack in daemon.get_master().get_stacks():
                for rasp in stack.get_pi_devices():
                    rasps[rasp.get_i2c()] = rasp
            return rasps



def json_response(response):
    return app.response_class(
        response=json.dumps(response),
        status=200,
        mimetype='application/json')


def getStack(id=None):

    def renderStack(stack):

        if DEBUG:
            return stack
        else:
            #rasps = stack.get_pi_devices()
            #master = daemon.get_master()
            
            stackJSON = {
                'heat': stack.get_heat(),
                'rasps': {}
            }
        
            for rasp in stack.get_pi_devices():
                print(rasp.get_pos())
                print(rasp.get_i2c())
                stackJSON['rasps'][rasp.get_pos()] = rasp.get_i2c()

            return stackJSON

    if id is not None:
        stack = getStackObject(id)
        if stack is not None:
            return renderStack(stack)
        else:
            return None
    else:
        stacks = getStackObject()
        for stackId in stacks:
            stacks[stackId] = renderStack(stacks[stackId])
        return stacks

def getRasp(id=None):

    def renderRasp(rasp):
        if DEBUG:
            return rasp
        else:
            print({
                'id': rasp.get_id(),
                'name' : 'Name',
                'address' : rasp.get_i2c(),
                'stack' : 1,
                'os' : rasp.get_os(),
                'ip' : rasp.get_ip_address(),
                'cpu' : rasp.get_cpu_usage(),
                'ram' : rasp.get_ram_usage()
            })
            return {
                'id': rasp.get_id(),
                'name' : 'Name',
                'address' : rasp.get_i2c(),
                'stack' : 1,
                'os' : rasp.get_os(),
                'ip' : rasp.get_ip_address(),
                'cpu' : rasp.get_cpu_usage(),
                'ram' : rasp.get_ram_usage()
            }

    if id is not None:
        rasp = getRaspObject(id)
        if rasp is not None:
            return renderRasp(rasp)
        else:
            return None
    else:
        rasps = getRaspObject()
        for raspId in rasps:
            rasps[raspId] = renderRasp(rasps[raspId])
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

@app.route("/stack/", defaults={'id':None})
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
    daemon.enable_alimentation_stack(id - 1)
    
    return json_response(json.dumps({'response': 1}))

@app.route('/stack/<int:id>/shutdown', methods=['POST'])
def stackShutdown(id):
    daemon.disable_alimentation_stack(id - 1)
    
    return json_response(json.dumps({'response': 1}))

# RASP GET

@app.route("/rasp/", defaults={'id':None})
@app.route("/rasp/<int:id>")
def routeRasp(id):
    rasp = getRasp(id)
    if rasp is None:
        rasp = {}
    return json_response(json.dumps(rasp))
        
# RASP ACTIONS

@app.route('/rasp/<int:id>/shutdown', methods=['POST'])
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
        
@app.route("/rasp/<int:id>/enable_i2c", methods=['POST'])
def enableI2C(id):
    rasp = daemon.get_master().get_slave_by_i2c(id)
    for device in daemon.get_master().get_stacks():
        for rasp in device.get_pi_devices():
            print(rasp.get_id())

    if rasp is not None:
        print("Telling slave to enable i2c")
        #daemon.get_udp_comm().send(daemon.get_commands().get_index("enable_i2c", rasp.get_ip_address())

    return json_response(json.dumps({'response': 1}))

@app.route("/rasp/<int:id>/disable_i2c", methods=['POST'])
def disableI2C(id):
    rasp = daemon.get_master().get_slave_by_i2c(id)

    if rasp is not None:
        print("Telling slave to disable i2c")
        #index = daemon.get_commands().get_index("disable_i2c")
        #daemon.get_i2c_comm().send_custom_instruction(rasp, index, str(index))

    return json_response(json.dumps({'response': 1}))

## RUN ##

if __name__ == '__main__':
    if not DEBUG:
        if (len(sys.argv) < 3):
            print("Please provide ip address as first parameter and orders file path as second")
            ip = sys.argv[1]
        else:
            daemon = Daemon(sys.argv[1], sys.argv[2])
            daemon.start()
            app.run(debug=True)
