#include <gtest/gtest.h>

#include <Eigen/Eigenvalues>
#include <cppsim/QASM.hpp>
#include <cppsim/circuit.hpp>
#include <cppsim/gate.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_matrix.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/gate_named_pauli.hpp>
#include <cppsim/observable.hpp>
#include <cppsim/pauli_operator.hpp>
#include <cppsim/state.hpp>
#include <cppsim/state_dm.hpp>
#include <cppsim/type.hpp>
#include <cppsim/utility.hpp>
#include <csim/constant.hpp>
#include <csim/update_ops.hpp>
#include <fstream>
#include <functional>
#include <vqcsim/parametric_circuit.hpp>

#include "../util/util.hpp"

TEST(QASMTest, QASMqulacs) {
    ParametricQuantumCircuit circuit(3);

    circuit.add_X_gate(0);
    circuit.add_CNOT_gate(0, 1);
    circuit.add_Z_gate(0);
    circuit.add_CZ_gate(2, 0);
    circuit.add_Y_gate(1);
    circuit.add_H_gate(2);
    circuit.add_Sdag_gate(0);
    circuit.add_S_gate(1);
    circuit.add_Tdag_gate(2);
    circuit.add_T_gate(0);
    circuit.add_sqrtXdag_gate(0);
    circuit.add_sqrtX_gate(1);
    circuit.add_RX_gate(0, 0.2);
    circuit.add_RY_gate(1, 0.3);
    circuit.add_RZ_gate(2, 0.4);
    circuit.add_parametric_RX_gate(0, 1.2);
    circuit.add_parametric_RY_gate(1, 1.3);
    circuit.add_parametric_RZ_gate(2, 1.4);

    auto str = qulacs_to_QASM(&circuit);

    std::cerr << str << std::endl;
}