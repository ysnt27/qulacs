import qulacs_core

class QuantumCircuitOptimizer:
    def __init__(self) -> None: ...
    def from_json(self) -> qulacs_core.QuantumCircuit: ...
    def merge_all(self, circuit: qulacs_core.QuantumCircuit) -> qulacs_core.QuantumGateMatrix: ...
    def optimize(self, circuit: qulacs_core.QuantumCircuit, block_size: int) -> None: ...
    def optimize_light(self, circuit: qulacs_core.QuantumCircuit) -> None: ...
