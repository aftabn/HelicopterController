using log4net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Helicopter.Controller
{
    public class Packet
    {
        private const string STR_CarriageNewline = "\r\n";
        private const string STR_Prompt = ">";
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public Packet()
        {
        }

        public string Command { get; set; }

        public string RawString { get; set; }

        public string ReturnValue { get; set; }

        public string Terminator { get; set; }

        public static Packet FromString(string text)
        {
            var packet = new Packet();
            packet.RawString = text;

            var pattern = @"^.+$";
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
            var list = new List<string>();

            foreach (Match line in matches)
            {
                list.Add(line.Value.Trim());
            }
            return list;
        }

        public static void ValidatePacket(List<string> list)
        {
            var linesCount = list.Count;
            if (linesCount < 2)
            {
                throw new Exception(String.Format("Not enough lines received in packet. Expected 2 or more and received {0}", linesCount));
            }

            if (!(list.Last() == "OK" || list.Last() == "ERROR"))
            {
                throw new Exception(String.Format("Received packet did not contain an Ack, Nack, LogMsg or DisableMsg"));
            }

            if (linesCount > 3)
            {
                throw new Exception(String.Format("Unsupported number of lines received. Expected 2 or 3 and received {0}", linesCount));
            }
        }
    }
}