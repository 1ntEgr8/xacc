/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once

#include "AllGateVisitor.hpp"
using namespace xacc::quantum;

namespace xacc {
class ControlledGateApplicator: public AllGateVisitor
{
public:
    // Apply *single* control on the input composite.
    // Returns the new composite.
    std::shared_ptr<xacc::CompositeInstruction> applyControl(const std::shared_ptr<xacc::CompositeInstruction>& in_program, int in_ctrlIdx);
    // AllGateVisitor implementation
    void visit(Hadamard& h) override;
    void visit(CNOT& cnot) override;
    void visit(Rz& rz) override;
    void visit(Ry& ry) override;
    void visit(Rx& rx) override;
    void visit(X& x) override;
    void visit(Y& y) override;
    void visit(Z& z) override;
    void visit(CY& cy) override;
    void visit(CZ& cz) override;
    void visit(Swap& s) override;
    void visit(CRZ& crz) override;
    void visit(CH& ch) override;
    void visit(S& s) override;
    void visit(Sdg& sdg) override;
    void visit(T& t) override;
    void visit(Tdg& tdg) override;
    void visit(CPhase& cphase) override;
    void visit(U& u) override;
    // Nothing to do
    void visit(Identity& i) override {}
    // These are unsupported.
    void visit(Measure& measure) override { xacc::error("Unsupported!"); }
    void visit(IfStmt& ifStmt) override { xacc::error("Unsupported!"); }
    void visit(fSim& fsim) override { xacc::error("Unsupported!"); }
    void visit(iSwap& isw) override { xacc::error("Unsupported!"); }
private:
    std::shared_ptr<xacc::CompositeInstruction> m_composite;
    std::shared_ptr<xacc::IRProvider> m_gateProvider;
    size_t m_ctrlIdx;
};
}