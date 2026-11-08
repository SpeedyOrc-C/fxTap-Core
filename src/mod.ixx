export module fxtap:mod;

namespace fxtap
{
	export class ModFlags
	{
		bool mirror_ : 1 = false;
		bool random_ : 1 = false;
		bool hidden_ : 1 = false;
		bool fade_in_ : 1 = false;
		bool no_release_ : 1 = false;
		bool hold_off_ : 1 = false;
		bool reverse_ : 1 = false;
		bool full_combo_ : 1 = false;
		bool perfect_ : 1 = false;

	public:
		[[nodiscard]] bool mirror() const { return mirror_; }
		[[nodiscard]] bool random() const { return random_; }
		[[nodiscard]] bool hidden() const { return hidden_; }
		[[nodiscard]] bool fade_in() const { return fade_in_; }
		[[nodiscard]] bool no_release() const { return no_release_; }
		[[nodiscard]] bool hold_off() const { return hold_off_; }
		[[nodiscard]] bool reverse() const { return reverse_; }
		[[nodiscard]] bool full_combo() const { return full_combo_; }
		[[nodiscard]] bool perfect() const { return perfect_; }

		void ToggleMirror()
		{
			mirror_ = !mirror_;
			if (mirror_) random_ = false;
		}

		void ToggleRandom()
		{
			random_ = !random_;
			if (random_) mirror_ = false;
		}

		void ToggleHidden()
		{
			hidden_ = !hidden_;
			if (hidden_) fade_in_ = false;
		}

		void ToggleFadeIn()
		{
			fade_in_ = !fade_in_;
			if (fade_in_) hidden_ = false;
		}

		void ToggleNoRelease()
		{
			no_release_ = !no_release_;
			if (no_release_) hold_off_ = false;
		}

		void ToggleHoldOff()
		{
			hold_off_ = !hold_off_;
			if (hold_off_) no_release_ = false;
		}

		void ToggleFullCombo()
		{
			full_combo_ = !full_combo_;
			if (full_combo_) perfect_ = false;
		}

		void TogglePerfect()
		{
			perfect_ = !perfect_;
			if (perfect_) full_combo_ = false;
		}
	};
}
