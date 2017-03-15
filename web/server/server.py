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
        'ip' : "192.168.0.1"
    },
    2 : {
        'name' : 'Straw',
        'address' : 2,
        'stack' : 1,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 0,
        'ip' : "192.168.0.2"
    },
    42 : {
        'name' : 'Blue',
        'address' : 42,
        'stack' : 2,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 1,
        'ip' : "192.168.0.42"
    }
}


## GETTERS ##

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


#######################################################################################
#                                                                                     #
#                                    VIEWS                                            #
#                                                                                     #
#######################################################################################

@app.route("/index")
@app.route("/")
@app.route("/view/rasp")
def routeDefault():
    return redirect("/view", code=302)

# TEST
@app.route("/view/test")
def viewTest():
    print("test")
    return render_template('test.html', my_string="Wheeeee!", my_list=[0,1,2,3,4,5])

@app.route("/view")
def viewDefault():
    return render_template('index.html', constants=constants)

@app.route('/view/stack/<int:id>')
def viewStack(id):
    if getStack(id) is not None:
        return render_template('stack.html', constants=constants, stackId=id)
    else:
        return routeDefault()

@app.route('/view/rasp/<int:id>')
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

@app.route("/test/")
def test():
    return app.response_class(
        response=json.dumps(daemon.get_master().get_ip_address()),
        status=200,
        mimetype='application/json')


## RUN ##

if __name__ == '__main__':
    daemon = Daemon()
    daemon.start()
    app.run(debug=True)
