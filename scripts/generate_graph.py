# PYTHON 3
import random
from random import randint

# PARAMETERS
DISTINCT_LABELS = 100
DISTINCT_KEYS = 200

NODES_GENERATED = 100000
EDGES_GENERATED = 500000

MIN_LABELS_NODES = 1
MAX_LABELS_NODES = 3

MIN_LABELS_EDGES = 1
MAX_LABELS_EDGES = 3

MIN_PROPERTIES_NODES = 2
MAX_PROPERTIES_NODES = 6

MIN_PROPERTIES_EDGES = 1
MAX_PROPERTIES_EDGES = 4

def select_random(list):
    return list[randint(0, len(list)-1)]

# seed random number generator
random.seed(1)

# READ WORDS
words = []
with open("/usr/share/dict/words", mode="r") as words_file:
    for line in words_file:
        words.append(line.strip())

def select_random_value():
    value_type = randint(0, 3)
    if value_type == 0:   # BOOL
        if randint(0, 1) == 0:
            return "true"
        else:
            return "false"
    elif value_type == 1: # INT
        return str(randint(0, 10000))
    elif value_type == 2: # FLOAT
        return str(random.uniform(0, 10000))
    else:                 # STRING
        return '"{}"'.format(select_random(words))

# SELECT RANDOM LABELS
labels = []
for i in range(DISTINCT_LABELS):
    random_label = select_random(words)
    while random_label in labels:
        random_label = select_random(words)
    labels.append(random_label)

# SELECT RANDOM KEYS
keys = []
for i in range(DISTINCT_KEYS):
    random_key = select_random(words)
    while random_key in keys:
        random_key = select_random(words)
    keys.append(random_key)

with open("nodes.txt", mode="w") as nodes_file:
    for i in range(NODES_GENERATED):
        nodes_file.write("({})".format(i+1))

        # generate random labels
        node_label_count = randint(MIN_LABELS_NODES, MAX_LABELS_NODES)
        labels_already_added = []
        for l in range(node_label_count):
            label = select_random(labels)
            while label in labels_already_added:
                label = select_random(labels)
            labels_already_added.append(label)
            nodes_file.write(" :{}".format(label))

        # generate random properties
        node_property_count = randint(MIN_PROPERTIES_NODES, MAX_PROPERTIES_NODES)
        keys_already_added = []
        for l in range(node_property_count):
            key = select_random(keys)
            while key in keys_already_added:
                key = select_random(keys)
            value = select_random_value()
            keys_already_added.append(key)
            nodes_file.write(" {}:{}".format(key, value))
        nodes_file.write("\n")

with open("edges.txt", mode="w") as edges_file:
    for i in range(EDGES_GENERATED):
        edges_file.write("({})->({})".format(randint(1, NODES_GENERATED), randint(1, NODES_GENERATED)))

        # generate random labels
        edge_label_count = randint(MIN_LABELS_EDGES, MAX_LABELS_EDGES)
        labels_already_added = []
        for l in range(edge_label_count):
            label = select_random(labels)
            while label in labels_already_added:
                label = select_random(labels)
            labels_already_added.append(label)
            edges_file.write(" :{}".format(label))

        # generate random properties
        edge_property_count = randint(MIN_PROPERTIES_EDGES, MAX_PROPERTIES_EDGES)
        keys_already_added = []
        for l in range(edge_property_count):
            key = select_random(keys)
            while key in keys_already_added:
                key = select_random(keys)
            keys_already_added.append(key)
            value = select_random_value()
            edges_file.write(" {}:{}".format(key, value))
        edges_file.write("\n")
