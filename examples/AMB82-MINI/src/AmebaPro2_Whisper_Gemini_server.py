## To run ASR server on PC: python AmebaPro2_whisper_llm_server.py
## To run LLM server on Colab
## To run client: RecordMP4_HTTP_Post_Audio.ino

# pip install git+https://github.com/openai/whisper.git
# pip install fastapi uvicorn
# pip install nest-asyncio
# pip install google.generativeai

import google.generativeai as genai
from fastapi import FastAPI, HTTPException
from fastapi.responses import Response
from pydantic import BaseModel
import uvicorn
import base64

GOOGLE_API_KEY="AIzaSyCnRzbxgrMX1GjIHnN7U6EQVM8YKy9Ikw4" ## https://aistudio.google.com/app/apikey
genai.configure(api_key=GOOGLE_API_KEY)

import whisper
ASR = whisper.load_model("base").to("cpu")

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
        header = "ASR: "
        print(header+result["text"])

        # Gemini 
        prompt = result["text"]
        model = genai.GenerativeModel("gemini-1.5-flash")
        result = model.generate_content( [prompt] )
        print(result.text)
        return Response(result.text)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=5000, log_level="info")
