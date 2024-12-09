from openai import OpenAI

# Initialize the OpenAI client
client = OpenAI(api_key="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") 

# 对话上下文列表
conversation = [
    {"role": "system", "content": "You are a helpful assistant."}  # 系统消息，定义助手角色
]

# 主程序
def chat_with_gpt():
    print("欢迎使用 GPT 连续对话！输入 'exit' 退出。\n")

    while True:
        # 用户输入
        user_input = input("你: ")
        if user_input.lower() == "exit":
            print("再见！")
            break
        
        # 将用户输入添加到上下文
        conversation.append({"role": "user", "content": user_input})

        try:
            # 调用 OpenAI API
            completion = client.chat.completions.create(
                model="gpt-4o",
                messages=conversation
            )

            # 获取助手回复
            assistant_reply = completion.choices[0].message.content
            print(f"GPT: {assistant_reply}")

            # 将助手回复添加到上下文
            conversation.append({"role": "assistant", "content": assistant_reply})

        except Exception as e:
            print(f"出错了: {e}")

# 启动对话
if __name__ == "__main__":
    chat_with_gpt()