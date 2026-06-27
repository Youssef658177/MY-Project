
from flask import Flask, jsonify

app = Flask(__name__)

@app.route('/api/graph', methods=['POST'])
def generate_graph():
    return jsonify({"status": "success", "graph": "base64_encoded_string_here"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
