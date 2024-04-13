# 10 image files needed to be converted into .tensor
# compress tensor folder to upload with the xxxx_cnn.h5

# python convert_image_to_tensor.py ~/Datasets/FER2013_clean/test/angry/PrivateTest_1862370.jpg > tensor/PrivateTest_1862370.tensor

import sys
import torch 
from PIL import Image 
import torchvision.transforms as transforms 
 
filename = sys.argv[1] 
image = Image.open(filename) 
  
# Define a transform to convert PIL  
# image to a Torch tensor 
transform = transforms.Compose([ 
    transforms.PILToTensor() 
]) 
  
# transform = transforms.PILToTensor() 
# Convert the PIL image to Torch tensor 
img_tensor = transform(image) 
  
# print the converted Torch tensor 
print(img_tensor) 
