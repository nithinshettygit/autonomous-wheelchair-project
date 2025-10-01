from flask import Flask, render_template, Response, request
import cv2
from model_inference import predict, load_model, preprocess_frame
import torch
from torchvision import transforms
from hardware_control import led  

app = Flask(__name__)

# Prediction Model Setup
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model_path = 'final_implementation/model/steering_control_model_tinyvgg.pth'
model = load_model(model_path, device)

transform = transforms.Compose([
    transforms.Resize((240, 640)),
    transforms.ToTensor(),
])

cap = None
is_running = False

def generate_frames():
    global is_running, cap
    try:
        while is_running:
            if cap is None or not cap.isOpened():
                cap = cv2.VideoCapture(2)  # Initialize the camera
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

            ret, frame = cap.read()
            if not ret:
                print("Error: Could not read frame from the camera.")
                break

            # Predict movement direction
            input_image = preprocess_frame(frame, transform, device)
            predicted_label = predict(model, input_image)

            if predicted_label != -1:
                print(f"Predicted Label: {predicted_label}")
                led(str(predicted_label))  

            cv2.putText(frame, f'Prediction: {predicted_label}', (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)

            # Encode frame for streaming
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

    finally:
        if cap is not None:
            cap.release()
            cap = None

@app.route('/')
def index():
    return render_template('home.html')

@app.route('/autonomous')
def autonomous():
    return render_template('autonomous.html')

@app.route('/manual')
def manual():
    return render_template('manual.html')

@app.route('/start')
def start_detection():
    global is_running, cap
    if not is_running:
        is_running = True
        if cap is None or not cap.isOpened():
            cap = cv2.VideoCapture(2)  
            cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
            cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    return '', 204

@app.route('/stop')
def stop_detection():
    global is_running, cap
    is_running = False
    if cap is not None:
        cap.release()
        cap = None
    return '', 204

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/manual_control', methods=['POST'])
def manual_control():
    command = request.form.get('command')  # Get the string command from the request

    command_map = {
        "Forward": 0,
        "Right": 1,
        "Left": 2,
        "Stop": 3,
        "Backward":4
    }
    
    if command in command_map:
        numeric_value = command_map[command]
        led(str(numeric_value))  
        print(f"Manual command received: {command} -> {numeric_value}")
    else:
        print(f"Invalid command received: {command}")
    return '', 204

if __name__ == '__main__':
    app.run(debug=True)
