import serial
import re
import random
from openai import OpenAI

# 初始化 OpenAI 客户端
client = OpenAI(api_key="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") 

# 图案字典
pattern_dict = {
    #"80": ["尖锐扭曲的闪光图案", "环绕点阵的放射线条", "不规则折线的能量符号"],
    #"81": ["闪烁星芒的对称图案", "光芒四射的能量核", "旋转闪耀的六芒星"],
    #"82": ["弧形流动的线条和散点环绕着圆核", "虚实相映的两个抽象人形环绕圆核", "两个人环绕成螺旋伸展的椭圆形态"],
    #"83": ["无面孔的环绕抽象舞者", "舞动环绕的抽象人形", "环绕祭祀的舞动的人"],
    #"84": ["一滴水落进荡漾水洼", "静止水滴滴落湖面"],
    #"85": ["落日时的水光", "飘逸的云雾", "破碎状的跃动火焰"],
    #"86": ["平衡对称生长的植物形态", "层层展开的叶状符号", "向上延伸逐层叠加的叶片"],
    #"87": ["波浪线环绕的花状圆形", "多重包裹的圆环图案", "层层递进的同心结构"],
    #"88": ["镂空对称的六边形几何结构", "内向旋转的抽象立方体", "连续线条秩序井然的图案"],
    #"89": ["四象限旋转的风车造型", "无限翻开的书页图案", "中心对称的扇页状几何开本"],
    #"7E": ["下坠俯冲的抽象鸟形", "延展羽翼的飞燕", "自由灵动的飞鸟图腾"],
    #"7F": ["奔跑中延展身体的马", "飞奔冲撞的独角兽", "斑点纹温驯独角兽"],
    #"8A": ["身上有花纹的兔子", "破碎的兔子", "一只原地停留的兔子"],
    #"8B": ["扑向翻滚圆球的小狗", "向上翻腾的敏捷狗影"]
    "80": ["Sharp and twisted flash pattern", "Radiating lines surrounding a dotted center", "Energy symbol of irregular zigzag lines"],
    "81": ["Symmetrical pattern of twinkling starbursts", "Radiating energy core with dazzling light", "Spinning and glowing hexagram"],
    "82": ["Curved flowing lines and scattered dots encircling a core", "Interwoven abstract figures encircling a core", "Two figures spiraling into an elliptical form"],
    "83": ["Faceless abstract dancers circling around", "Abstract human shapes dancing in circles", "Ritualistic dancers forming a surrounding motion"],
    "84": ["A droplet falling into a rippling puddle", "A still water droplet rippling on a lake"],
    "85": ["The glimmer of water at sunset", "Fragmente leaping flames", "The graceful drift of mist and clouds"],  
    "86": ["Symmetrical and balanced plant growth patterns", "Layered unfolding leaf-like symbols", "Gradually stacked upward leaf extensions"],  
    "87": ["Wave-like lines encircling a floral circular design", "Multiple-layered wrapped circular patterns", "Progressively layered concentric structures"],  
    "88": ["Hollow symmetrical hexagonal geometric structure", "Inward-rotating abstract cube", "Ordered patterns of continuous lines"],  
    "89": ["Four-quadrant rotating windmill shape", "Infinitely unfolding book pages pattern", "Centrally symmetrical fan-shaped geometric folio"],  
    "7E": ["Abstract bird shape diving downward", "A swallow extending its wings", "Free and agile bird totem"],  
    "7F": ["A unicorn stretching its body while running", "A unicorn charging forward", "Dappled gentle unicorn"],  
    "8A": ["A rabbit with patterned fur", "A fragmented rabbit", "A rabbit standing still"],
    "8B": ["A puppy pouncing on a rolling ball", "An agile dog silhouette flipping upwards  "]  
}

# 对话上下文
conversation = [
    {"role": "system", "content": "You are a helpful assistant."}
]

def process_card_data(serial_port, baud_rate):
    try:
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            print(f"Listening on {serial_port}...")

            while True:
                # 读取串口数据
                line = ser.readline().decode('utf-8').strip()
                if not line:
                    continue

                # 匹配格式：MBTI: XXXX Card: 0424XX
                match = re.match(r"MBTI: ([A-Z]{4}) Card: 0424([A-F0-9]{2})", line)
                if match:
                    mbti = match.group(1)  # 提取 MBTI
                    tail_digits = match.group(2)  # 提取卡片编号后两位

                    if tail_digits in pattern_dict:
                        random_pattern = random.choice(pattern_dict[tail_digits])  # 随机选择描述
                        formatted_output = f"{{{mbti}}}{{{random_pattern}}}"  # 格式化输出
                        print(f"Card Processed: {formatted_output}")  # 打印到控制台

                        # 生成用户输入和请求 GPT
                        #gpt_input = f"{formatted_output}\n1. You are an experienced sandplay analyst, well-versed in Jungian theory. Using archetypal theory (inner concepts, thinking patterns, psychological images, etc.) in conjunction with {mbti}, analyze the {random_pattern} chosen by the player , and provide gentle yet profound interpretations and suggestions.\n2. Offer feedback on the player's choices with empathy and a non-judgmental tone, encouraging them to fully embrace their emotions and experiences. The text must be concise, no more than 150 words, without explicitly mentioning specific MBTI types, and should be expressed poetically.\n3. The response structure is as follows:  [Restatement of {random_pattern}]/n[Incorporation of personality interpretation]/n[Poetic analysis]/n[Rhetorical or reflective question]."
                        gpt_input = f"{formatted_output}\n1. You are an experienced sandplay therapist, well-versed in Jungian theory. Using archetypal theory (inner concepts, thinking patterns, psychological images, etc.) in conjunction with {mbti}, analyze the image:{random_pattern}, chosen by the player and provide gentle yet profound interpretations and suggestions.\n2. Offer feedback on the player's choices with empathy and a non-judgmental tone, encouraging them to specifically embrace their feelings and experiences.The text should be concise, no more than 150 words.  \n3. The structure of the response should be as follows: [“Your MBTI is {mbti}, and the pattern you selected represents {random_pattern}”]/n [Incorporate personality interpretation] /n [Poetic analysis] /n [Rhetorical or reflective question][;)]."
                        gpt_response = ask_gpt(gpt_input)  # 调用 GPT 生成回复

                        # 打印 GPT 的结果
                        print(f"GPT Response: \n{gpt_response}")

                        # 返回数据到串口（可选）
                        ser.write((gpt_response + '\n').encode('utf-8'))
                    else:
                        print(f"Code {tail_digits} 未定义")
                else:
                    print("Received invalid input format. Waiting for valid data...")

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("Exiting program.")

def ask_gpt(user_input):
    try:
        # 将用户输入添加到上下文
        conversation.append({"role": "user", "content": user_input})

        # 调用 OpenAI API
        completion = client.chat.completions.create(
            model="gpt-4o",
            messages=conversation
        )

        # 提取 GPT 回复
        assistant_reply = completion.choices[0].message.content

        # 将回复添加到上下文
        conversation.append({"role": "assistant", "content": assistant_reply})

        return assistant_reply

    except Exception as e:
        return f"GPT 出错了: {e}"

if __name__ == "__main__":
    # 串口配置
    SERIAL_PORT = '/dev/cu.usbmodem1301'  # 根据实际设备修改
    BAUD_RATE = 9600

    # 启动主程序
    process_card_data(SERIAL_PORT, BAUD_RATE)