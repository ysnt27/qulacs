#pragma once

#include <regex>

#include "../vqcsim/parametric_circuit.hpp"
#include "../vqcsim/parametric_gate.hpp"
#include "circuit.hpp"
#include "gate.hpp"
#include "gate_factory.hpp"
#include "type.hpp"
/**
 * \~japanese-en 回路をQASMに変換する。
 * 現在のところ、CNOT CZ SWAP X Y Z H S Sdag T Tdag sqrtX sqrtXdag RX RY RZ
 * のみ対応している。 parametrixなRXRYRZも普通のRXRYRZ として扱う。
 * qulacsのRXとQASMのRXが定義逆なのであわせている。注意。
 *
 * @param[in] qubit_index ターゲットとなる量子ビットの添え字
 * @return 作成されたゲートのインスタンス
 */
std::string qulacs_to_QASM(const QuantumCircuit* cir) {
    std::stringstream out_strs;
    out_strs << "OPENQASM 2.0;\n";
    out_strs << "include \"qelib1.inc\";\n";
    out_strs << "qreg q[" << std::to_string(cir->qubit_count) << "];\n";
    std::string now_str;

    for (auto gate : cir->gate_list) {
        auto clis = gate->get_control_index_list();
        auto tlis = gate->get_target_index_list();

        if (gate->get_name() == "CNOT") {
            now_str = "cx q[" + std::to_string(clis[0]) + "],q[" +
                      std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "CZ") {
            now_str = "cz q[" + std::to_string(clis[0]) + "],q[" +
                      std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "SWAP") {
            now_str = "swap q[" + std::to_string(tlis[0]) + "],q[" +
                      std::to_string(tlis[1]) + "];";
        } else if (gate->get_name() == "X") {
            now_str = "x q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Y") {
            now_str = "y q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Z") {
            now_str = "z q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "H") {
            now_str = "h q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "S") {
            now_str = "s q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Sdag") {
            now_str = "sdg q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "T") {
            now_str = "t q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Tdag") {
            now_str = "tdg q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "sqrtX") {
            now_str = "sx q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "sqrtXdag") {
            now_str = "sxdg q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "X-rotation") {
            auto rotgate = dynamic_cast<QuantumGate_OneQubitRotation*>(gate);
            now_str = "rx(" + std::to_string(-rotgate->get_angle()) + " ) q[" +
                      std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Y-rotation") {
            auto rotgate = dynamic_cast<QuantumGate_OneQubitRotation*>(gate);
            now_str = "ry(" + std::to_string(-rotgate->get_angle()) + " ) q[" +
                      std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "Z-rotation") {
            auto rotgate = dynamic_cast<QuantumGate_OneQubitRotation*>(gate);
            now_str = "rz(" + std::to_string(-rotgate->get_angle()) + " ) q[" +
                      std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "ParametricRX") {
            auto rotgate = dynamic_cast<QuantumGate_SingleParameter*>(gate);
            now_str = "rx(" + std::to_string(-rotgate->get_parameter_value()) +
                      " ) q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "ParametricRY") {
            auto rotgate = dynamic_cast<QuantumGate_SingleParameter*>(gate);
            now_str = "ry(" + std::to_string(-rotgate->get_parameter_value()) +
                      " ) q[" + std::to_string(tlis[0]) + "];";
        } else if (gate->get_name() == "ParametricRZ") {
            auto rotgate = dynamic_cast<QuantumGate_SingleParameter*>(gate);
            now_str = "rz(" + std::to_string(-rotgate->get_parameter_value()) +
                      " ) q[" + std::to_string(tlis[0]) + "];";
        } else {
            throw NotImplementedException("unkonwn gate:" + gate->get_name());
        }
        out_strs << now_str << "\n";
    }

    return out_strs.str();
}
