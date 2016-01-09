/// 固定長小数クラス
/**
 * ゲームではintをシフトして小数を表現するのが常識らしいので、
 * 気持ち悪いシフト演算を隠蔽するクラスを作ってみようかと。
 * 入力は double で、出力は int 。
 * 内部表現は int だが小数も扱える、とのこと。
 * int は 32 ビットじゃないと困ると思う。
 * シフト数はテンプレート引数で指定。
 * 当然 VC では通らんです。
 */
#ifndef GAMENUM_H_INCLUDED
#define GAMENUM_H_INCLUDED

template <int shifts_, typename Int_ =int>
class GameNum {
private:
	/// コンパイルタイムなシフト変換。
	/**
	 * 実行速度に影響しない。
	 */
	//@{
	template <bool equal_, bool leftLarge_,
			  int leftShifts_, int rightShifts_>
	struct ConvertHelper_;
	template <int leftShifts_, int rightShifts_>
	struct ConvertHelper_<true, false, leftShifts_, rightShifts_> {
		static Int_ run(Int_ v) {
			return v;
		}
	};
	template <int leftShifts_, int rightShifts_>
	struct ConvertHelper_<false, true, leftShifts_, rightShifts_> {
		static Int_ run(Int_ v) {
			return v << leftShifts_ - rightShifts_;
		}
	};
	template <int leftShifts_, int rightShifts_>
	struct ConvertHelper_<false, false, leftShifts_, rightShifts_> {
		static Int_ run(Int_ v) {
			return v >> rightShifts_ - leftShifts_;
		}
	};
	template <int leftShifts_, int rightShifts_>
	Int_ convert_(Int_ v) const {
		return ConvertHelper_
			<(leftShifts_ == rightShifts_), (leftShifts_ > rightShifts_),
			leftShifts_, rightShifts_>::run(v);
	}
	//@}

	static Int_ convDouble(double v) {
		return static_cast<Int_>(v * (1 << shifts_));
	}

public:
	GameNum() : val(0) {}
	explicit GameNum(double v) : val(convDouble(v)) {}
	explicit GameNum(Int_ v) : val(v << shifts_) {}
	template <int shifts2_>
	explicit GameNum(GameNum<shifts2_, Int_> v)
		: val(convert_<shifts_, shifts2_>(v.val)) {}

public:
	operator Int_() const { return getVal(); }
	Int_ getVal() const { return val >> shifts_; }
	double getDouble() const { return (double)val / (1 << shifts_); }
	Int_ getShiftedVal() const { return val; }

public:
	GameNum& operator = (Int_ v) {
		val = v << shifts_;
		return *this;
	}
	template <int shifts2_>
	GameNum& operator = (GameNum<shifts2_, Int_> v) {
		val = convert_<shifts_, shifts2_>(v.val);
		return *this;
	}

	GameNum& operator += (Int_ v) {
		val += v << shifts_;
		return *this;
	}
	template <int shifts2_>
	GameNum& operator += (GameNum<shifts2_, Int_> v) {
		val += convert_<shifts_, shifts2_>(v.val);
		return *this;
	}
	GameNum& operator -= (Int_ v) {
		val -= v << shifts_;
		return *this;
	}
	template <int shifts2_>
	GameNum& operator -= (GameNum<shifts2_, Int_> v) {
		val -= convert_<shifts_, shifts2_>(v.val);
		return *this;
	}
	GameNum& operator *= (Int_ v) {
		val *= v;
		return *this;
	}
	template <int shifts2_>
	GameNum& operator *= (GameNum<shifts2_, Int_> v) {
		val *= v.val;
		val >>= shifts2_;
		return *this;
	}
	GameNum& operator /= (Int_ v) {
		val /= v;
		return *this;
	}
	template <int shifts2_>
	GameNum& operator /= (GameNum<shifts2_, Int_> v) {
		val <<= shifts2_;
		val /= v.val;
		return *this;
	}

	GameNum operator + (Int_ rhs) {
		GameNum ret(*this);
		ret += rhs;
		return ret;
	}
	friend inline GameNum operator + (Int_ lhs, GameNum rhs) {
		return rhs.operator + (lhs);
	}
	template <int shifts2_>
	GameNum operator + (GameNum<shifts2_, Int_> rhs) {
		GameNum ret(*this);
		ret += rhs;
		return ret;
	}
	GameNum operator - (Int_ rhs) {
		GameNum ret(*this);
		ret -= rhs;
		return ret;
	}
	friend inline GameNum operator - (Int_ lhs, GameNum rhs) {
		GameNum ret(lhs);
		ret -= rhs;
		return ret;
	}
	template <int shifts2_>
	GameNum operator - (GameNum<shifts2_, Int_> rhs) {
		GameNum ret(*this);
		ret -= rhs;
		return ret;
	}
	GameNum operator * (Int_ rhs) {
		GameNum ret(*this);
		ret *= rhs;
		return ret;
	}
	friend inline GameNum operator * (Int_ lhs, GameNum rhs) {
		return rhs.operator * (lhs);
	}
	template <int shifts2_>
	GameNum operator * (GameNum<shifts2_, Int_> rhs) {
		GameNum ret(*this);
		ret *= rhs;
		return ret;
	}
	GameNum operator / (Int_ rhs) {
		GameNum ret(*this);
		ret /= rhs;
		return ret;
	}
	friend inline GameNum operator / (Int_ lhs, GameNum rhs) {
		GameNum ret(lhs);
		ret /= rhs;
		return ret;
	}
	template <int shifts2_>
	GameNum operator / (GameNum<shifts2_, Int_> rhs) {
		GameNum ret(*this);
		ret /= rhs;
		return ret;
	}
	bool operator > (Int_ rhs) {
		return operator>(GameNum(rhs));
	}
	template <int shifts2_>
	bool operator > (GameNum<shifts2_, Int_> rhs) {
		return val > convert_<shifts_, shifts2_>(rhs.val);
	}
	bool operator < (Int_ rhs) {
		return operator<(GameNum(rhs));
	}
	template <int shifts2_>
	bool operator < (GameNum<shifts2_, Int_> rhs) {
		return val < convert_<shifts_, shifts2_>(rhs.val);
	}
	bool operator == (Int_ rhs) {
		return operator==(GameNum(rhs));
	}
	template <int shifts2_>
	bool operator == (GameNum<shifts2_, Int_> rhs) {
		return val == convert_<shifts_, shifts2_>(rhs.val);
	}
	bool operator <= (Int_ rhs) {
		return operator<=(GameNum(rhs));
	}
	template <int shifts2_>
	bool operator <= (GameNum<shifts2_, Int_> rhs) {
		return val <= convert_<shifts_, shifts2_>(rhs.val);
	}
	bool operator >= (Int_ rhs) {
		return operator>=(GameNum(rhs));
	}
	template <int shifts2_>
	bool operator >= (GameNum<shifts2_, Int_> rhs) {
		return val >= convert_<shifts_, shifts2_>(rhs.val);
	}

public:
	/// 直接いじりたくなることもあるでしょうし…
	Int_ val;

}; // end of class GameNum

#endif /* GAMENUM_H_INCLUDED */
