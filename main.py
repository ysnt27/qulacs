import pickle

from qulacs import StateVector

v = StateVector(2)
with open('state.pickle', 'wb') as f:
    pickle.dump(v, f)
with open('state.pickle', 'rb') as f:
    v2 = pickle.load(f)
print(v2)
print(v2)
print(v2)
