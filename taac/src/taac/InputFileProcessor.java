package taac;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import taac.assembly.Compiler;
import taac.assembly.CompilerFailedException;
import taac.intermediate.IntermediateProgram;

public class InputFileProcessor {
	public static IntermediateProgram process(String fileName)
			throws InputProcessingException {
		final String ending = fileName.substring(fileName.lastIndexOf('.'),
				fileName.length());

		if (ending.equals(".asm")) {
			try {
				final String content = readFileIntoString(fileName);
				return new Compiler().compile(content);
			} catch (IOException e) {
				throw new InputProcessingException("File " + fileName
						+ ", IO error: " + e.getMessage());
			} catch (CompilerFailedException e) {
				throw new InputProcessingException("Compiler failed with " + e.errorCount + " errors");
			}
		} else {
			throw new InputProcessingException("Unknown input file ending "
					+ ending);
		}
	}

	private static String readFileIntoString(String fileName)
			throws IOException {
		File file = new File(fileName);
		InputStream in = new FileInputStream(file);
		byte[] b = new byte[(int) file.length()];
		int len = b.length;
		int total = 0;

		while (total < len) {
			int result = in.read(b, total, len - total);
			if (result == -1) {
				break;
			}
			total += result;
		}

		return new String(b);
	}
}
