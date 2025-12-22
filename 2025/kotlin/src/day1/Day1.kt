package day1

import java.io.File
import java.nio.file.Paths

class Day1 {

    var overnul = 0
    var beginwaarde = 50


    data class Direction(val dir: Char, var amount: Int)

    private fun part1(input: List<Direction>) {

        for (d in input) {
            println(" - $d")
            d.amount = d.amount % 100

            if (d.dir == 'L') {
                beginwaarde -= d.amount
                if (beginwaarde < 0) {
                    beginwaarde = beginwaarde + 100
                } else
                    beginwaarde = beginwaarde

            } else {
                beginwaarde += d.amount
                if (beginwaarde > 100) {
                    beginwaarde = beginwaarde - 100
                }
                else
                    beginwaarde = beginwaarde}


            if (beginwaarde == 0)
                overnul++
            println("overnul = $overnul ")
            println("beginwaarde = $beginwaarde")

        }
    }


    private fun part2(input: List<Direction>) {
        // TODO
        println("Part 2: ")
    }

    private fun getInput(file: String) : List<Direction> {
        val result = ArrayList<Direction>()
        val f = File(file)
        if (f.exists()) {
            val lines = f.readLines()
            for (line in lines) {
                val d = Direction(
                    line[0],
                    line.substring(1).toInt()
                )
                result.add(d)
            }
        } else {
            System.err.println("File $f does not exist")
        }
        return result
    }

    fun run() {

        val rootDir = Paths.get("").toAbsolutePath().toString()
        val testFile = Paths.get(rootDir, "src\\day1\\test.txt").toString()
        val inputFile = Paths.get(rootDir, "src\\day1\\input.txt").toString()

        val testInput = getInput(testFile)
        val realInput = getInput(inputFile)

        part1(testInput)
        //part2(testInput)

    }

}
