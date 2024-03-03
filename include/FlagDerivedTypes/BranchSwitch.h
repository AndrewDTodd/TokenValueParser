#ifndef BRANCH_SWITCH_FLAG_H
#define BRANCH_SWITCH_FLAG_H

#include <BranchFlag.h>
#include <SwitchFlag.h>

namespace CmdLineParser
{
	class BranchSwitch : public BranchFlag<Arg_Bool>, public SwitchFlag
	{
	public:
		BranchSwitch(std::string&& flagToken, std::string&& flagDesc,
			bool defaultSwitchState = false, bool flagRequired = false)
#ifndef _DEBUG
			noexcept
#endif // !_DEBUG
			: Flag<Arg_Bool>(Tokens(std::move(flagToken)), std::move(flagDesc), flagRequired),
			BranchFlag<Arg_Bool>(std::move(flagToken), std::move(flagDesc), flagRequired),
			SwitchFlag(std::move(flagToken), std::move(flagDesc), defaultSwitchState, flagRequired)
		{}

		template<FlagType... Flags>
		BranchSwitch(std::string&& flagToken, std::string&& flagDesc, Flags&&... subFlags,
			bool defaultSwitchState = false, bool flagRequired = false)
#ifndef _DEBUG
			noexcept
#endif // !_DEBUG
			: Flag<Arg_Bool>(Tokens(std::move(flagToken)), std::move(flagDesc), flagRequired),
			  BranchFlag<Arg_Bool>(std::move(flagToken), std::move(flagDesc), std::forward<Flags>(subFlags)..., flagRequired),
			  SwitchFlag(std::move(flagToken), std::move(flagDesc), defaultSwitchState, flagRequired)
		{}

		template<FlagType... Flags>
		BranchSwitch& SetSubFlags(Flags&&... subFlags) noexcept
		{
			_nestedFlags.clear();
			(_nestedFlags.emplace_back(std::forward<Flags>(subFlags)), ...);

			return *this;
		}

		void Raise(std::vector<std::string_view>::const_iterator& itr, const std::vector<std::string_view>::const_iterator end) override
		{
			SwitchFlag::Raise(itr, end);

			for (auto& subFlag : _nestedFlags)
			{
				subFlag.Raise(itr, end);
			}
		}

		bool TryRaise(std::vector<std::string_view>::const_iterator& itr, const std::vector<std::string_view>::const_iterator end, std::string* errorMsg = nullptr) noexcept override
		{
			SwitchFlag::TryRaise(itr, end, errorMsg);

			for (auto& subFlag : _nestedFlags)
			{
				if (!subFlag.TryRaise(itr, end, errorMsg))
					return false;
			}

			return true;
		}

		BranchSwitch(const BranchSwitch&) = delete;
		BranchSwitch(BranchSwitch&&) = delete;

		BranchSwitch& operator=(const BranchSwitch&) = delete;
		BranchSwitch& operator=(BranchSwitch&&) = delete;
	};
}
#endif // !BRANCH_SWITCH_FLAG_H
