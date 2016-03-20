using log4net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace Helicopter.Core.Controller
{
    public class Packet
    {
        private const string STR_Prompt = ">";
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public string Command { get; set; }

        public string RawString { get; set; }

        public string ReturnValue { get; set; }

        public string Terminator { get; set; }

        public static Packet FromString(string text)
        {
            var packet = new Packet { RawString = text };

            const string pattern = @"^.+$";
            var matches = Regex.Matches(text, pattern, RegexOptions.IgnoreCase | RegexOptions.Multiline);
            var lines = PreProcessPacket(matches);
            ValidatePacket(lines);

            var linesCount = lines.Count;

            packet.Command = lines.First().Substring(STR_Prompt.Length);
            packet.Terminator = lines.Last();

            if (linesCount == 3)
            {
                packet.ReturnValue = lines[1];
            }

            return packet;
        }

        public static List<string> PreProcessPacket(MatchCollection matches)
        {
            var list = (matches.Cast<Match>().Select(line => line.Value.Trim())).ToList();

            return list;
        }

        public static void ValidatePacket(List<string> list)
        {
            var linesCount = list.Count;
            if (linesCount < 2)
            {
                throw new Exception($"Not enough lines received in packet. Expected 2 or more and received {linesCount}");
            }

            if (!(list.Last() == "OK" || list.Last() == "ERROR"))
            {
                throw new Exception("Received packet did not contain an OK or ERROR");
            }

            if (linesCount > 3)
            {
                throw new Exception($"Unsupported number of lines received. Expected 2 or 3 and received {linesCount}");
            }
        }
    }
}