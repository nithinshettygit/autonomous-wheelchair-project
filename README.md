# Autonomous Wheelchair Project

This project implements an autonomous wheelchair system with CNN based Lane detection and path prediction 
## Features
- Lane detection and movement prediction
- Manual and Autonomous control modes
-  Flask-based user interface

## Directory Structure
Project/
├── app.py                    # Main application file
├── hardware_control.py       # NodeMCU command sender (HTTP)
├── model_inference.py        # Model load/preprocess/predict helpers
├── templates/                # HTML templates for Flask
│   ├── home.html
│   ├── autonomous.html
│   └── manual.html
├── static/                   # Static assets like CSS and JS
│   ├── css/
│   │   └── styles.css
│   └── js/                   # (unused; removed)
├── final_implementation/     # Model files and related assets
│   └── model/
│       └── steering_control_model_tinyvgg.pth
├── requirements.txt          # Dependencies
└── README.md                 # Project documentation
## Run

1. Create and activate a virtual environment.
2. Install dependencies: `pip install -r requirements.txt`
3. Start the app: `python app.py`

Ensure the camera index in `app.py` suits your environment (default 2).


Read more : https://www.notion.so/Autonomous-Wheelchair-Using-Deep-Learning-217fe523d0b480d5b960fa2e51e2910e
