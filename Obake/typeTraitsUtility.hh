namespace Obake {
	template<typename InputType_>
		struct __remove_reference__ {
			typedef InputType_ type;
		};

	template<typename InputType_>
		struct __remove_reference__<InputType_&> {
			typedef InputType_ type;
		};

	template<typename InputType_>
		struct __remove_reference__<InputType_&&> {
			typedef InputType_ type;
		};
}
