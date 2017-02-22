from flask import Flask, render_template, json, redirect
import copy, collections
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
            'master' : 1,
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
        #'stack' : 0,
        #'heat' : '70',
        'os' : 'Pidora',
        'status' : 1,
        'ip' : "192.168.0.1"
    },
    2 : {
        'name' : 'Straw',
        'address' : 2,
        #'stack' : 0,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 0,
        'ip' : "192.168.0.2"
    },
    42 : {
        'name' : 'Blue',
        'address' : 42,
        #'stack' : 0,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 1,
        'ip' : "192.168.0.42"
    }
}



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



def computeStack(stack):
    raspsIn = stack['rasps']
    raspsOut = collections.OrderedDict()

    raspsOut['master'] = raspsIn['master'] if raspsIn.get('master') is not None else -1

    for i in range(constants['nSlavesByStack']):
        if raspsIn.get(i) is not None:
            raspsOut[i] = raspsIn[i]
        else:
            raspsOut[i] = -1

    stack['rasps'] = raspsOut

    return stack

def computeStacks(stacks):
    output = collections.OrderedDict(sorted(stacks.items(), key=lambda t: t[0]))

    for elt in output:
        output[elt] = computeStack(output[elt])

    return output

    


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


@app.route("/view/test")
def viewTest():
    print("test")
    return render_template('test.html', my_string="Wheeeee!", my_list=[0,1,2,3,4,5])

@app.route("/view")
def viewDefault():
    return render_template('index.html', constants=constants, stacks=computeStacks(getStack()), rasps=getRasp())

@app.route('/view/rasp/<int:id>')
def viewRasp(id):
    rasp = getRasp(id)
    if rasp is not None:
        return render_template('rasp.html', constants=constants, rasp=rasp)
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



if __name__ == '__main__':
    app.run(debug=True)