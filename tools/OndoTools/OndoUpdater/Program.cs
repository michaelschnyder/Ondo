using System;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Makaretu.Dns;


namespace OndoUpdater
{
    class Program
    {
        async Task Bla()
        {


            var result = await Download(null);
            Console.WriteLine("Done");

            var awaiter = Download(null).GetAwaiter();

            if (waiter.IsCompleted
        }


        static async Task<String> Download(string[] args)
        {

            

            var service = "_arduino._tcp.local";
            var query = new Message();
            query.Questions.Add(new Question { Name = service, Type = DnsType.ANY });
            var cancellation = new CancellationTokenSource(2000);

            using (var mdns = new MulticastService())
            {
                mdns.Start();
                var response = await mdns.ResolveAsync(query, cancellation.Token);

                Console.WriteLine("New Response");
                Console.WriteLine("-------------------------------------");
                Console.WriteLine(response.ToString());
                Console.WriteLine("-------------------------------------\n");


            }

            Console.ReadLine();
        }
    }
}
