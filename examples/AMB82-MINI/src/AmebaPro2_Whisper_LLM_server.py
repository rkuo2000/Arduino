## To run server: python AmebaPro2_whisper_llm_server.py
## To run client: RecordMP4_HTTP_Post_Audio.ino

# pip install git+https://github.com/openai/whisper.git
# pip install fastapi uvicorn
# pip install nest-asyncio
# pip install accelerate

from fastapi import FastAPI, HTTPException
from fastapi.responses import Response
from pydantic import BaseModel
import uvicorn
import base64

import whisper
ASR = whisper.load_model("base").to("cpu")

import torch
import transformers
from transformers import AutoModelForCausalLM , AutoTokenizer

### https://huggingface.co/spaces/HuggingFaceH4/open_llm_leaderboard

#model_name = "Q-bert/Mamba-130M"
#model_name = "Q-bert/Mamba-370M"
#model_name = "Q-bert/Mamba-790M"
#model_name = "Q-bert/Mamba-1B"
#model_name = "Q-bert/Mamba-3B"
#model_name = "Q-bert/Mamba-3B-slimpj"
#model_name = "mtgv/MobileLLaMA-1.4B-Chat"
#model_name = "TinyLlama/TinyLlama-1.1B-Chat-v1.0"
#model_name = "microsoft/phi-2"
#model_name = "microsoft/Phi-3-mini-4k-instruct"
#model_name = "microsoft/Phi-3-mini-128k-instruct"
#model_name = "microsoft/Orca-2-7b"
#model_name = "google/gemma-1.1-2b-it"
#model_name = "google/gemma-1.1-7b-it"
#model_name = "mistralai/Mistral-7B-Instruct-v0.2"
#model_name = "kaist-ai/mistral-orpo-beta" # fine-tuned mistral-7B-v0.1
#model_name = "openlm-research/open_llama_3b_v2"
#model_name = "openlm-research/open_llama_7b_v2"
#model_name = "lmsys/vicuna-7b-v1.5"
#model_name = "lmsys/vicuna-7b-v1.5-16k"
#model_name = "Nexusflow/Starling-LM-7B-beta"
#model_name = "meta-llama/Llama-2-7b-hf"
#model_name = "meta-llama/Llama-2-7b-chat-hf"
model_name = "meta-llama/Meta-Llama-3-8B-Instruct"

#model_name = "ckip-joint/bloom-3b-zh" 
#model_name = "Qwen/Qwen1.5-7B-Chat" # 通义千问
#model_name = "01-ai/Yi-6B-Chat" # 零一万物
#model_name = "yentinglin/Taiwan-LLM-7B-v2.0.1-chat" # 台大
#model_name = "MediaTek-Research/Breeze-7B-Instruct-v1_0" # 達哥
#model_name = "MediaTek-Research/Breeze-7B-32k-Instruct-v1_0" # 達哥
#model_name = "INX-TEXT/Bailong-instruct-7B" # 白龍
#model_name = "taide/TAIDE-LX-7B" # TAIDE
#model_name = "taide/TAIDE-LX-7B-Chat" # TAIDE
#model_name = "taide/Llama3-TAIDE-LX-8B-Chat-Alpha1" # TIADE
print(model_name)

#LLM = AutoModelForCausalLM.from_pretrained(model_name, trust_remote_code=True, torch_dtype="auto", device_map="cuda")
LLM = AutoModelForCausalLM.from_pretrained(model_name, trust_remote_code=True, torch_dtype=torch.bfloat16, device_map="cuda")
tokenizer = AutoTokenizer.from_pretrained(model_name)

import nest_asyncio
nest_asyncio.apply()

app = FastAPI()

class Base64Data(BaseModel):
    base64_string: str

@app.get("/")
def home():
    return Response("hello")

@app.post("/audio")
async def post_audio(data: Base64Data):
    try:
        #Decode the base64 string
        decoded_data = base64.b64decode(data.base64_string)
        
        # print(decoded_data)
        #Save the decoded data to an MP4 file
        with open("output.mp4", "wb") as f:
            f.write(decoded_data)
      
        # Whisper transcribe
        result = ASR.transcribe("output.mp4",fp16=False)
        header1 = "ASR: "
        print(header1+result["text"])

        # LLM generate
        prompt = result["text"]
        input_ids = tokenizer.encode(prompt, return_tensors="pt").to("cuda")
        output = LLM.generate(input_ids, max_length=128, num_beams=5, no_repeat_ngram_size=2, pad_token_id=tokenizer.eos_token_id, do_sample=True)
        generated_text = tokenizer.decode(output[0], skip_special_tokens=True)
        header2="LLM: "
        print(header2+generated_text) 
        return Response(header2+generated_text)
        #return Response(header1+prompt+"\n"+header2+generated_text)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=5000, log_level="info")
