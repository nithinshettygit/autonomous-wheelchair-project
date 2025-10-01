import requests

def led(command):
    try:
        url = f"http://192.168.65.141/LED={command}" 
        response = requests.get(url, timeout=2)
        if response.status_code == 200:
            print(f"Command {command} sent successfully.")
        else:
            print(f"Failed to send command {command}. Status code: {response.status_code}")
    except Exception as e:
        print(f"Error sending command to NodeMCU: {e}")