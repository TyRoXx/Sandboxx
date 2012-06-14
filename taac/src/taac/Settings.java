package taac;

import java.util.HashMap;
import java.util.Map;

public class Settings {
	boolean hasError = false;
	String inputFileName;
	Map<String, String> outputFilesByType = new HashMap<String, String>();
	long initialStackSize = 32;
	boolean hasAutoResizeStack = true;
	boolean hasRuntimeChecks = true;

	public Settings(String[] commandLine) {
		for (String arg : commandLine) {
			if (arg.startsWith("-")) {
				if (arg.length() >= 3) {
					final char option = arg.charAt(1);
					final String optionArg = arg.substring(2, arg.length());

					switch (option) {
					case 'c':
						outputFilesByType.put("c", optionArg);
						break;

					case 's':
						try {
							initialStackSize = Long.parseLong(optionArg);
						} catch (NumberFormatException e) {
							hasError = true;
							System.out.println("Invalid initial stack size");
						}
						break;
						
					case 'r':
						hasRuntimeChecks = (optionArg.charAt(0) == '1');
						break;
						
					case 'a':
						hasAutoResizeStack = (optionArg.charAt(0) == '1');
						break;

					default:
						System.out.println("Unknown option " + option);
						hasError = true;
						break;
					}
				} else {
					hasError = true;
					System.out.println("Invalid option");
				}
			} else {
				if (inputFileName != null) {
					System.out
							.println("Warning: Input file name is overridden");
				}
				inputFileName = arg;
			}
		}
	}
}
