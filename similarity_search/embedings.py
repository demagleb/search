from sentence_transformers import SentenceTransformer
import torch

class Text2Embeding:
    DIMENSION = 768
    def __init__(self):
        self.model = SentenceTransformer("all-mpnet-base-v2")

    def get_embeding(self, text : str):
        return self.model.encode(text).reshape(1, -1)


print(Text2Embeding().get_embeding("hello hellow").shape)
