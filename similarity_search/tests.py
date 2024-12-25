from datasets import load_dataset
from requests import get
from index import SimularitySearchIndex

def get_texts():
    ds = load_dataset("SetFit/bbc-news")
    texts = {}
    id = 0
    for row in ds["train"]:
        texts[id] = row["text"]
        id += 1
    for row in ds["test"]:
        texts[id] = row["text"]
        id += 1
    return texts

texts = get_texts()

def test_index():
    index = SimularitySearchIndex(texts)
    print(index.search("NLP", 2))

test_index()
