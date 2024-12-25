from torch import embedding
from embedings import Text2Embeding
import faiss
import numpy as np

class SimularitySearchIndex:
    def __init__(self, texts: dict):
        self.text2embeding = Text2Embeding()

        pca = faiss.PCAMatrix(self.text2embeding.DIMENSION, 512)
        flat = faiss.IndexFlatL2(512)
        hnsw = faiss.IndexHNSW(flat, 32)
        index = faiss.IndexPreTransform(pca, hnsw)
        index = faiss.IndexRefineFlat(index)
        self.index = faiss.IndexIDMap(index)

        ids = []
        embeddings = []
        for id, text in texts.items():
            ids.append(id)
            embeddings.append(self.text2embeding.get_embeding(text))
        embeddings = np.concatenate(embeddings)
        self.index.train(embeddings)
        self.index.add_with_ids(embeddings, ids)

    def add(self, text: str, id: int):
        embedding = self.text2embeding.get_embeding(text)
        self.index.add_with_ids(embedding, [id])

    def search(self, text: str, k: int):
        embedding = self.text2embeding.get_embeding(text)
        return self.index.search(embedding, k=k)[1]
