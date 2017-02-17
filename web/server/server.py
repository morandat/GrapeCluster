from flask import Flask, render_template
app = Flask(__name__)

## CONSTANTS ##

constants = {
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
        'master' : 0,
        'slaves' : [
            -1,
            -1,
            1,
            -1,
            -1,
            -1
        ]
    },
    2 : {
        'heat' : 25,
        'master' : -1,
        'slaves' : [
            -1,
            -1,
            -1,
            42,
            -1,
            -1
        ]
    }
}

raspsTest = {
    0 : {
        'name' : 'Cran',
        'address' : 0,
        'stack' : 0,
        #'heat' : '70',
        'os' : 'Pidora',
        'status' : 1
    },
    1 : {
        'name' : 'Straw',
        'address' : 1,
        'stack' : 0,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 0
    },
    42 : {
        'name' : 'Blue',
        'address' : 42,
        'stack' : 0,
        #'heat' : '65',
        'os' : 'Raspbian',
        'status' : 1
    }
}

## WEBSITE ##

@app.route("/test")
def template_test():
    return render_template('template.html', my_string="Wheeeee!", my_list=[0,1,2,3,4,5])

@app.route("/index")
@app.route("/")
def template_debug():
    return render_template('index.html', constants=constants, stacks=stacksTest, rasps=raspsTest)

@app.route('/rasp/<int:raspAddr>')
def template_details(raspAddr):
    rasp = raspsTest[raspAddr]
    return render_template('rasp.html', constants=constants, rasp=rasp)

## REST ##

@app.route("/test/welcome")
def api_test():
    print("test")
    return "Welcome"

## RUN ##
if __name__ == '__main__':
    app.run(debug=True)