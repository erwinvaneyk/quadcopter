void processKeyboardInput(struct *INPUT keyboardInput) {
	int maxLevel; // TODO: to some definition in a header file
	maxLevel = 15;

	if ((c = term_getchar_nb()) != -1) {
		switch(c) {
			case 'a':
				if (keyboardInput->lift < maxLevel) {	
					keyboardInput->lift++;
				}
				break;
			case 'z':
				if (keyboardInput->lift > -maxLevel) {
					keyboardInput->lift--;
				}

		}
	}
}