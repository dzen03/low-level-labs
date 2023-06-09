%define POINTER_TO_NEXT 0

%macro colon 2
	%ifid %2
		%2: dq POINTER_TO_NEXT
		%define POINTER_TO_NEXT %2
	%else
		%error "Not an id"
	%endif

	dq %2.val;

	%ifstr %1
		db %1, 0
	%else
		%error "Not a string"
	%endif
	%2.val:
%endmacro