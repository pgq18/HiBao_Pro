import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
import base64
import hashlib
import hmac
import json
import numpy as np
import cv2
import os
import time
import requests
import urllib
from openai import OpenAI
from socket import *
import threading
import re

# 配置
API_BASE = "https://api.lingyiwanwu.com/v1"
API_KEY = "1307407b40e84609bacc8bea0ac476ab"
HISTORY_FILE_PATH = "G:/voice/tcp_client/history.txt"

client = OpenAI(
    api_key=API_KEY,
    base_url=API_BASE
)

# 发件人和收件人信息
sender_email = "2301053818@qq.com"
receiver_email = "1585424325@qq.com"
password = "xawgdkwyxpzaebdd"
last_yuv_file_path=""

def send_email_with_attachment(subject, body, attachment_path, attachment_name):
    # 创建邮件
    message = MIMEMultipart()
    message["From"] = sender_email
    message["To"] = receiver_email
    message["Subject"] = subject

    # 添加邮件正文
    message.attach(MIMEText(body, "plain"))

    # 添加附件
    with open(attachment_path, "rb") as attachment:
        part = MIMEApplication(attachment.read(), Name=attachment_name)
        part["Content-Disposition"] = f'attachment; filename="{attachment_name}"'
        message.attach(part)

    # 连接到SMTP服务器
    with smtplib.SMTP_SSL("smtp.qq.com", 465) as server:
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, message.as_string())

    print("带附件的邮件已发送成功！")


def find_largest_file_in_folder(folder_path):
    # 获取文件夹中的所有文件
    files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]

    # 过滤出文件名为数字的文件
    numeric_files = [f for f in files if f.split('.')[0].isdigit()]

    if not numeric_files:
        raise ValueError("No numeric files found in the folder")

    # 找到名称最大的文件
    largest_file = max(numeric_files, key=lambda x: int(x.split('.')[0]))

    # 返回该文件的绝对路径
    largest_file_path = os.path.join(folder_path, largest_file).replace("\\", "/")
    # print(largest_file_path)
    return largest_file_path

# Part 1: YUV to PNG Conversion
def yuv_to_png(yuv_path, output_file, image_type='NV21', height=384, width=640):
    with open(yuv_path, 'rb') as f:
        yuvdata = np.fromfile(f, dtype=np.uint8)

        if image_type == 'NV21':
            cv_format = cv2.COLOR_YUV2BGR_NV21

        if yuvdata.size == 307200:
            bgr_img = yuvdata.reshape((height, width))
        else:
            bgr_img = cv2.cvtColor(yuvdata.reshape((height * 3 // 2, width)), cv_format)

    cv2.imwrite(output_file, bgr_img)
    # print(f"保存成功：{output_file}")


# Part 2: Xunfei API for Speech Transcription
lfasr_host = 'https://raasr.xfyun.cn/v2/api'
api_upload = '/upload'
api_get_result = '/getResult'


class RequestApi(object):
    def __init__(self, appid, secret_key, upload_file_path):
        self.appid = appid
        self.secret_key = secret_key
        self.upload_file_path = upload_file_path
        self.ts = str(int(time.time()))
        self.signa = self.get_signa()

    def get_signa(self):
        appid = self.appid
        secret_key = self.secret_key
        m2 = hashlib.md5()
        m2.update((appid + self.ts).encode('utf-8'))
        md5 = m2.hexdigest()
        md5 = bytes(md5, encoding='utf-8')
        signa = hmac.new(secret_key.encode('utf-8'), md5, hashlib.sha1).digest()
        signa = base64.b64encode(signa)
        signa = str(signa, 'utf-8')
        return signa

    def upload(self):
        # print("上传部分：")
        upload_file_path = self.upload_file_path
        file_len = os.path.getsize(upload_file_path)
        file_name = os.path.basename(upload_file_path)

        param_dict = {
            'appId': self.appid,
            'signa': self.signa,
            'ts': self.ts,
            "fileSize": file_len,
            "fileName": file_name,
            "duration": "200"
        }
        # print("upload参数：", param_dict)
        data = open(upload_file_path, 'rb').read(file_len)

        response = requests.post(url=lfasr_host + api_upload + "?" + urllib.parse.urlencode(param_dict),
                                 headers={"Content-type": "application/json"}, data=data)
        # print("upload_url:", response.request.url)
        result = json.loads(response.text)
        # print("upload resp:", result)
        return result

    def get_result(self):
        uploadresp = self.upload()
        orderId = uploadresp['content']['orderId']
        param_dict = {
            'appId': self.appid,
            'signa': self.signa,
            'ts': self.ts,
            'orderId': orderId,
            'resultType': "transfer,predict"
        }
        # print("")
        # print("查询部分：")
        # print("get result参数：", param_dict)
        status = 3
        # 建议使用回调的方式查询结果，查询接口有请求频率限制
        while status == 3:
            response = requests.post(url=lfasr_host + api_get_result + "?" + urllib.parse.urlencode(param_dict),
                                     headers={"Content-type": "application/json"})
            result = json.loads(response.text)
            # print(result)
            status = result['content']['orderInfo']['status']
            # print("status=", status)
            if status == 4:
                break
            time.sleep(5)
        # print("get_result resp:", result)
        return result

def convert_history_to_messages(conversation_history):
    messages = []
    for entry in conversation_history:
        message = {
            "role": entry["role"],
            "content": [
                {
                    "type": "text",
                    "text": entry["content"][0]["text"]
                }
            ]
        }
        messages.append(message)
    return messages

def get_transcribed_text(result):
    order_result = json.loads(result["content"]["orderResult"])
    lattice = order_result["lattice"]

    transcribed_texts = []
    for item in lattice:
        json_1best_str = item["json_1best"]
        json_1best = json.loads(json_1best_str)

        for rt in json_1best["st"]["rt"]:
            words = [cw["w"] for ws in rt["ws"] for cw in ws["cw"]]
            transcribed_text = "".join(words)
            transcribed_texts.append(transcribed_text)

    final_transcribed_text = "".join(transcribed_texts)
    return final_transcribed_text


# 读取对话历史
def load_conversation_history():
    try:
        with open(HISTORY_FILE_PATH, "r", encoding="utf-8") as file:
            content = file.read().strip()
            if not content:
                return []
            return json.loads(content)
    except FileNotFoundError:
        return []


# 保存对话历史
def save_conversation_history(history):
    with open(HISTORY_FILE_PATH, "w", encoding="utf-8") as file:
        json.dump(history, file, ensure_ascii=False, indent=4)
# 清空对话历史
def clear_conversation_history():
    with open(HISTORY_FILE_PATH, "a+", encoding="utf-8") as file:
        file.truncate(0)
    print("对话历史已清空")
    global conversation_history
    conversation_history=[]

# 初始化对话历史
conversation_history = load_conversation_history()

# Part 3: OpenAI API for Image Description
def get_image_description(transcribed_text, image_path):
    global conversation_history
    # global history
    completion = client.chat.completions.create(
        model="yi-large",
        messages=[
            {"role": "user",
             "content": """
                   1、你是一个断句助手
                   2、把“用户输入”中所有的标点符号去除
                   3、把去除了标点符号的“用户输入”分成小短句，只拆分，不生成新的内容，并分点输出
                   4、输出格式可参考示例：
                   “”1. 番茄去皮并切成小块。
                   2. 鸡蛋打散备用。
                   3. 热锅凉油，放入番茄翻炒。
                   4. 番茄变软出汁后，加入鸡蛋，继续翻炒至熟。
                   5. 加入适量盐调味。
                   6. 可以撒上葱花增加香味。“”
                   """.replace("用户输入", transcribed_text)
             }
        ],
        stream=True
    )
    responses = []
    for chunk in completion:
        responses.append(chunk.choices[0].delta.content or "")
    responses_str = "".join(responses)
    responses_list = responses_str.split("\n")
    # print("responses_list:\n")
    # print(responses_list)
    keywords = ["发", "传", "送", "寄", "递"]
    contains_send = any(any(keyword in item for keyword in keywords) for item in responses_list)
    if contains_send:
        new_transcribed_text = "，".join(
            re.sub(r"^\d+\.\s*", "", item) for item in responses_list if
            all(x not in item for x in ["发", "传", "送", "寄", "递"])
        )
        send = 1
    else:
        new_transcribed_text = "，".join(
            re.sub(r"^\d+\.\s*", "", item) for item in responses_list
        )
        send = 0
    # print("new_transcribed_text:\n")
    # print(new_transcribed_text)
    with open(image_path, "rb") as image_file:
        image = "data:image/jpeg;base64," + base64.b64encode(image_file.read()).decode('utf-8')

    # 追加不包含 image_url 的用户消息到对话历史
    conversation_history.append({
        "role": "user",
        "content": [
            {
                "type": "text",
                "text": transcribed_text + "，用中文回答"
            }
        ]
    })
    # history=convert_history_to_messages(conversation_history)
    # print(conversation_history)
    message_with_image = [
        {
            "role": "user",
            "content": [
                {
                    "type": "text",
                    "text": "你好"
                },
                {
                    "type": "image_url",
                    "image_url": {
                        "url": image
                    }
                }
            ]
        },
        {
            "role": "assistant",
            "content": "你好！有什么我可以帮助你的吗"
        },
        * conversation_history
    ]
    # 调用 API
    stream = client.chat.completions.create(
        model="yi-vision",
        messages=message_with_image,
        stream=True
    )


    text = ""
    for part in stream:
        content = part.choices[0].delta.content or ""
        text += content

    # 生成要追加到对话历史中的助手回复，但不包含 image_url
    assistant_message = [{
            "role": "assistant",
            "content": text
        }]

    # 追加不包含 image_url 的助手回复到对话历史
    conversation_history.append(assistant_message[0])

    # 保存对话历史
    save_conversation_history(conversation_history)

    return text, new_transcribed_text, send

# Part 4: Send Text to Server
def send_text(text, server_ip, server_port):
    tcpClientSocket = socket(AF_INET, SOCK_STREAM)
    # print("create")
    serverAddress = (server_ip, server_port)
    tcpClientSocket.connect(serverAddress)
    # print("connect")

    tcpClientSocket.send(text.encode("gb2312"))
    print("send")
    # receiveData = tcpClientSocket.recv(1024)
    # print("接收到的数据为%s" % receiveData.decode("gb2312"))

    # tcpClientSocket.close()
# 客户端处理
def handle_client(client_socket, client_address):
    global last_yuv_file_path
    try:
        print(f"Connected to {client_address}")
        while True:
            # 接收客户端消息
            data = client_socket.recv(1024)
            if not data:
                break
            # print(f"接收到的数据为：{data.decode('gb2312')}")

            # 处理 YUV 转换
            yuv_file_path = find_largest_file_in_folder("D:/nfs/image")
            # if not last_yuv_file_path.startswith("G:/voice/image"):
            #     last_yuv_file_path = ""
            # print("1")
            # print(last_yuv_file_path)
            # print("2")
            # print(yuv_file_path)
            if yuv_file_path != last_yuv_file_path:
                clear_conversation_history()
                last_yuv_file_path = yuv_file_path
            png_file_path = "G:/voice/tcp_client/image/image.png"
            # print(os.path.getsize(yuv_file_path))
            while True:
                if os.path.getsize(yuv_file_path) != 360 * 1024:
                    continue
                else:
                    break
            yuv_to_png(yuv_file_path, png_file_path)
            print("图片已成功保存为 PNG 格式")

            # 处理语音识别
            upload_file = "D:/nfs/audio_chn0.aac"
            while True:
                if os.path.getsize(upload_file) == 0:
                    continue
                else:
                    break
            api = RequestApi(appid="2a7e8f90", secret_key="2f258debbaa8c462de38b7e32e42ec4a",
                             upload_file_path=upload_file)
            result = api.get_result()
            transcribed_text = get_transcribed_text(result)

            # transcribed_text="描述一下这张图片,发邮件给我"
            # 获取图像描述和新的转录文本
            image_description, new_transcribed_text, send = get_image_description(transcribed_text,png_file_path)
            # print(new_transcribed_text)
            print(image_description)
            # 判断 transcribed_text 是否等于 new_transcribed_text
            if send == 0:
                # print(f"Transcribed Text: {transcribed_text}")
                send_text(image_description, "192.168.215.114", 6000)
            else:
                send_email_with_attachment("text and image", image_description, png_file_path, "output.png")
                send_text("邮件发送完成", "192.168.215.114", 6000)
            last_yuv_file_path=yuv_file_path
            # print("3")
            # print(last_yuv_file_path)

    except Exception as e:
        print(f"Error: {e}")
    finally:
        client_socket.close()


def run_server():
    server_ip = "192.168.215.127"
    server_port = 6000

    tcpServerSocket = socket(AF_INET, SOCK_STREAM)
    tcpServerSocket.bind((server_ip, server_port))
    tcpServerSocket.listen(5)
    print(f"Server listening on {server_ip}:{server_port}")
    while True:
        client_socket, client_address = tcpServerSocket.accept()
        client_handler = threading.Thread(target=handle_client, args=(client_socket, client_address))
        client_handler.start()


# Main Execution
if __name__ == '__main__':
    run_server()

