package cz.cuni.mff.kolej.b214.testscrgen;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.Vector;

public class TestScriptGenerator {

	// FLAT xor SPACE
	// MINEDGECONN xor PEREDGECONN
	// NEAREST
	public static final String[] gFlags = {
			"FLAT MINEDGECONN NEAREST",
			"FLAT MINEDGECONN",
			"FLAT PEREDGECONN NEAREST",
			"FLAT PEREDGECONN",
			"SPACE MINEDGECONN NEAREST",
			"SPACE MINEDGECONN",
			"SPACE PEREDGECONN NEAREST",
			"SPACE PEREDGECONN" };
	
	public static final String[] gAlg = {
			//"dijkstra",
			//"dijkstraheap",
			"dijkstraheapgeom"};//,
			//"bellmanford",
			//"bellmanfordlimit",
			//"bellmanfordgeom" };

	//public static final int[] gVertCounts = { 15000, 25000 };
	public static final int[] gVertCounts = { 5000 };
	//public static final int[] gVertCounts = { 1000, 5000, 10000};

	public static final int[] gLenAdd = { 25 };

	public static final int[] gPerEdgeConn = { 5 };

	public static final int[] gMinEdgeConn = { 150 };

	// public static final int[] gTestCount = { 10, 250, 1000 }; // pocet merani
	public static final int[] gTestCount = { 1 }; // pocet merani

	public static void main(String[] args) {
		// davky budu rozdelene po algoritmoch, prepinacoch a poctoch merani
		// 6x8x3 davok
		String lAlgoritmus = null;
		String lSwitches = null;
		String lSwitches2 = null;
		int lTestCount = 0;

		Vector lScripts = new Vector();

		for (int lAlg = 0; lAlg < gAlg.length; lAlg++) {
			lAlgoritmus = gAlg[lAlg];
			for (int lF = 0; lF < gFlags.length; lF++) {
				lSwitches = gFlags[lF];
				lSwitches2 = lSwitches.replace(' ', '_');
				for (int lTc = 0; lTc < gTestCount.length; lTc++) {
					lTestCount = gTestCount[lTc];
					String lScriptName = lAlgoritmus + lSwitches2 + lTestCount;
					boolean lsmedges = true;

					/* Tu sa vygeneruje jeden skript */
					try {
						PrintWriter lO = new PrintWriter(new FileWriter(new File(lScriptName + ".txt")));

						lO.println(lScriptName);

						int[] lEdges = null;
						if (lSwitches.matches(".*PEREDGECONN.*")) {
							lEdges = gPerEdgeConn;
							lsmedges = false;
						}
						if (lSwitches.matches(".*MINEDGECONN.*")) {
							lEdges = gMinEdgeConn;
							lsmedges = true;
						}

						for (int lV = 0; lV < gVertCounts.length; lV++) {
							for (int lEadd = 0; lEadd < gLenAdd.length; lEadd++) {
								for (int lEdgs = 0; lEdgs < lEdges.length; lEdgs++) {
									for (int rpc = 0; rpc < lTestCount; rpc++) {
										if ((lsmedges) && (gVertCounts[lV] <= lEdges[lEdgs]))
											continue;// nemoze byt
										lO.println(lAlgoritmus);
										lO.println(lSwitches);
										lO.println("" + gVertCounts[lV] + ' ' + gLenAdd[lEadd] + ' ' + lEdges[lEdgs]
												+ ' ' + (int) (Math.random() * Integer.MAX_VALUE));
									}
								}
							}
						}

						lO.print("END");
						lO.close();

						lScripts.add(lScriptName);
					} catch (IOException e) {
						e.printStackTrace();
					}
					/**/
				}/* lTc */
			}/* lF */
		}/* lAlg */

		/* Vygeneruje spustaciu davku */
		try {
			PrintWriter lO = new PrintWriter(new FileWriter(new File("RunALL.bat")));

			lO.println("\n\nIF EXIST zrus.to del zrus.to\n");
			lO.println(":START\n");

			Iterator lI = lScripts.iterator();
			String lStr = null;
			while (lI.hasNext()) {
				lStr = (String)lI.next();
				lO.println("IF EXIST zrus.to GOTO END");
				lO.println("IF EXIST " + lStr + ".log GOTO SKIPTEST" + lStr);
				lO.println("PathFinding.exe " + lStr + ".txt > " + lStr + ".log");
				lO.println(":SKIPTEST" + lStr+"\n");
			}

			lO.println(":END");
			lO.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		/**/

	}
}
