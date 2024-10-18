#include "sqlcpp/components/cond.hpp"
#include <stdexcept>
namespace sqlcpp {

    CondOp Cond::AND(const Condition &c) { return CondOp(sqlcpp::AND, {this, std::move(c)}); }
    CondOp Cond::OR(const Condition &c) { return CondOp(sqlcpp::OR, {this, std::move(c)}); }
    CondOp Cond::NOT() { return CondOp(sqlcpp::NOT, {this}); }


    CondCmp::CondCmp(FieldLike field, CmpOp op, ValueLike value)
        : field_(std::move(field)), op_(op), value_(std::move(value)) {}
    void CondCmp::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        oss << cmp_op_str(op_);
        value_.build_s(oss, t);
    }


    CondIn::CondIn(FieldLike field, std::vector<ValueLike> values)
        : field_(std::move(field)), values_(std::move(values)) {}
    void CondIn::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        oss << " IN (";
        const auto sz = values_.size();
        for (size_t i = 0; i < sz; ++i) {
            values_[i].build_s(oss, t);
            if (i < sz - 1) oss << ", ";
        }
        oss << ')';
    }


    CondNotIn::CondNotIn(FieldLike field, std::vector<ValueLike> values)
        : field_(std::move(field)), values_(std::move(values)) {}
    void CondNotIn::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        oss << " NOT IN (";
        const auto sz = values_.size();
        for (size_t i = 0; i < sz; ++i) {
            values_[i].build_s(oss, t);
            if (i < sz - 1) oss << ", ";
        }
        oss << ')';
    }


    CondBetween::CondBetween(FieldLike field, ValueLike start, ValueLike end)
        : field_(std::move(field)), start_(std::move(start)), end_(std::move(end)) {}
    void CondBetween::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        oss << " BETWEEN ";
        start_.build_s(oss, t);
        oss << " AND ";
        end_.build_s(oss, t);
    }


    CondRaw::CondRaw(std::string raw_cond) : raw_cond_(std::move(raw_cond)) {}
    void CondRaw::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << raw_cond_;
    }


    CondOp::CondOp(const CondOpType &op) : op_(&op), subs_() {}
    CondOp::CondOp(const CondOpType &op, std::vector<std::shared_ptr<Condition>> subs) : op_(&op), subs_(std::move(subs)) {}
    CondOp::CondOp(const CondOpType &op, std::initializer_list<Condition> subs) : op_(&op), subs_() {
        subs_.reserve(subs.size());
        for (const auto &c: subs) {
            subs_.emplace_back(std::make_shared<Condition>(std::move(c)));
        }
    }
    CondOp &CondOp::add(Condition c) {
        subs_.emplace_back(std::make_shared<Condition>(std::move(c)));
        return *this;
    }
    void CondOp::build_s(std::ostream &oss, const Type &t) const {
        if (sqlcpp::NOT == op_) {
            if (subs_.size() != 1) throw std::invalid_argument("NOT operator can only have one sub condition");
            if (!subs_[0]) throw std::invalid_argument("NOT operator can not have null sub condition");
            oss << "NOT(";
            subs_[0]->build_s(oss, t);
            oss << ')';
        } else {
            if (subs_.empty()) throw std::invalid_argument("empty condition is not allowed");
            const auto sz = subs_.size();
            for (size_t i = 0; i < sz; ++i) {
                oss << '(';
                subs_[i]->build_s(oss, t);
                oss << ')';
                if (i < sz - 1) oss << op_->op_str_;
            }
        }
    }


    Condition::Condition(std::variant<CondOp, CondCmp, CondIn, CondBetween, CondRaw> cond) : conditions_(cond){};
    Condition::Condition(CondOp cond) : conditions_(cond){};
    Condition::Condition(CondCmp cond) : conditions_(cond){};
    Condition::Condition(CondIn cond) : conditions_(cond){};
    Condition::Condition(CondBetween cond) : conditions_(cond){};
    Condition::Condition(CondRaw cond) : conditions_(cond){};
    Condition::Condition(const Cond *cond) : Condition(CAST(cond)){};

    Condition Condition::CAST(const Cond *cond) {
        if (auto ptr = dynamic_cast<const CondOp *>(cond); ptr) return Condition(*ptr);
        if (auto ptr = dynamic_cast<const CondCmp *>(cond); ptr) return Condition(*ptr);
        if (auto ptr = dynamic_cast<const CondIn *>(cond); ptr) return Condition(*ptr);
        if (auto ptr = dynamic_cast<const CondBetween *>(cond); ptr) return Condition(*ptr);
        if (auto ptr = dynamic_cast<const CondRaw *>(cond); ptr) return Condition(*ptr);
        throw std::invalid_argument(std::string{"unknown condition type: "} + typeid(*cond).name());
    }
    void Condition::build_s(std::ostream &oss, const Type &t) const {
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, conditions_);
    }
}// namespace sqlcpp
