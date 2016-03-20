using System;
using System.IO;
using System.Text;
using System.Xml.Linq;

namespace Libs.Utilities
{
    public static class XmlExtensions
    {
        public static double ParseDouble(this XAttribute xAttribute)
        {
            if (xAttribute == null)
            {
                throw new Exception("Attribute is null");
            }

            return Double.Parse(xAttribute.Value);
        }

        public static int ParseInt(this XAttribute xAttribute)
        {
            if (xAttribute == null)
            {
                throw new Exception("Attribute is null");
            }

            return Int32.Parse(xAttribute.Value);
        }

        public static double? ParseNullableDouble(this XAttribute xAttribute)
        {
            return xAttribute == null ? (double?)null : Double.Parse(xAttribute.Value);
        }

        public static bool ParseOptionalBoolean(this XAttribute xAttribute, bool defaultValue = false)
        {
            return xAttribute == null ? defaultValue : Boolean.Parse(xAttribute.Value);
        }

        public static double ParseOptionalDouble(this XAttribute xAttribute, double defaultValue = 0.0d)
        {
            return xAttribute == null ? defaultValue : Double.Parse(xAttribute.Value);
        }

        public static int ParseOptionalInt(this XAttribute xAttribute, int defaultValue = 0)
        {
            return xAttribute == null ? defaultValue : Int32.Parse(xAttribute.Value);
        }

        public static string ParseString(this XAttribute xAttribute)
        {
            return xAttribute?.Value;
        }

        public static string ParseOptionalString(this XAttribute xAttribute, string defaultValue)
        {
            return xAttribute?.Value ?? defaultValue;
        }

        public static string ToStringWithXmlDeclaration(this XDocument xDocument)
        {
            var stringBuilder = new StringBuilder();

            using (TextWriter writer = new StringWriter(stringBuilder))
            {
                xDocument.Save(writer);
            }

            return stringBuilder.ToString();
        }
    }
}