package taac;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Map.Entry;

import taac.backends.CGenerator;
import taac.intermediate.IntermediateProgram;

public class TAAC {
	public static void main(String[] args) {
		System.out.println("Evaluating command line");
		Settings settings = new Settings(args);
		if (settings.hasError) {
			return;
		}

		if (settings.inputFileName == null) {
			System.out.println("No input file");
			return;
		}

		System.out.println("Processing input file " + settings.inputFileName);

		IntermediateProgram program;
		try {
			program = InputFileProcessor.process(settings.inputFileName);
		} catch (InputProcessingException e) {
			System.out.println(e.getMessage());
			return;
		}

		for (Entry<String, String> output : settings.outputFilesByType
				.entrySet()) {
			final String type = output.getKey();
			final String fileName = output.getValue();

			System.out.println("Generating output");

			if (type.equals("c")) {
				final CGenerator generator = new CGenerator(program,
						settings.hasRuntimeChecks, settings.initialStackSize,
						settings.hasAutoResizeStack);
				final String code = generator.generate();

				try {
					BufferedWriter writer = new BufferedWriter(new FileWriter(
							new File(fileName)));

					writer.write(code);
					writer.close();
				} catch (IOException e) {
					System.out.println(e);
				}
			} else {
				System.out.println("Unknown target " + type);
			}
		}
	}
}
