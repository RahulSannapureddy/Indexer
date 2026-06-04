import os
import subprocess
import webbrowser
from threading import Timer
from flask import Flask, request, jsonify, render_template
from flask_cors import CORS

# 1. Get the absolute path of the directory where app.py lives
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

app = Flask(__name__, 
            template_folder=os.path.join(BASE_DIR, 'templates'),
            static_folder=os.path.join(BASE_DIR, 'static'))
CORS(app)

# 2. Fix the path to the C++ engine 
# It moves up one level to 'src', then down into 'engine_cpp'
engine_path = os.path.join(BASE_DIR, '..', 'engine_cpp', 'search_engine')

# Start the C++ process
cpp_process = subprocess.Popen([engine_path], 
                               stdin=subprocess.PIPE, 
                               stdout=subprocess.PIPE, 
                               text=True,
                               bufsize=1)

# Read the initial load time benchmark
load_time_line = cpp_process.stdout.readline().strip()
if load_time_line.startswith("LOAD_TIME_MS|"):
    print(f"C++ Engine Loaded. {load_time_line}")
else:
    print("Warning: Unexpected output from C++ engine during startup.")

@app.route('/')
def index():
    return render_template('index.html') 

@app.route('/search')
def search():
    query = request.args.get('q', '').strip()
    if not query:
        return jsonify({"results": [], "time": 0})

    # Send query to C++ engine
    cpp_process.stdin.write(query + "\n")
    cpp_process.stdin.flush()

    results = []
    search_time = 0
    while True:
        line = cpp_process.stdout.readline().strip()
        
        # Sentinel check
        if line == "END_OF_RESULTS" or not line:
            break
            
        if "|" in line:
            parts = line.split('|')
            if parts[0] == "TIME_MS":
                try:
                    search_time = float(parts[1])
                except ValueError:
                    search_time = 0
            else:
                # Handle ID|URL
                results.append({
                    "id": parts[0], 
                    "url": parts[1]
                })

    return jsonify({
        "results": results,
        "time": search_time
    })

def open_browser():
    webbrowser.open_new("http://127.0.0.1:5000")

if __name__ == '__main__':
    # Opens browser 1 second after server starts
    Timer(1, open_browser).start()
    # Setting debug=False is safer when using subprocess pipes
    app.run(port=5000, debug=False)