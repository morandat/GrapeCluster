from flask import Flask, render_template
app = Flask(__name__)


@app.route("/test")
def template_test():
    return render_template('template.html', my_string="Wheeeee!", my_list=[0,1,2,3,4,5])

@app.route("/index")
@app.route("/")
def template_debug():
    raspberries={
        1:{
            'name':'Cran',
            'heat':'70',
            'os':'Pidora',
            'state':'On'
        },
        2:{
            'name':'Straw',
            'heat':'65',
            'os':'Raspbian',
            'state':'On'
        }
    }
    return render_template('index.html', raspberries=raspberries)

@app.route("/test/welcome")
def api_test():
    print("test")
    return "Welcome"

if __name__ == '__main__':
    app.run(debug=True)