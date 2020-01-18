using System;
using System.Collections.ObjectModel;
using System.Net.Http;
using Newtonsoft.Json;
using Ondo.Api.Devices;

namespace Ondo.WearableApp
{
    class WebApiService
    {
        public async System.Threading.Tasks.Task<ObservableCollection<DeviceDto>> RefreshDataAsync()
        {
            var url = "https://ondo-sea.azurewebsites.net/api/devices";
            try
            {
                HttpClient client = new HttpClient();
                var response = await client.GetAsync(url);

                if (response.IsSuccessStatusCode)
                {
                    var content = await response.Content.ReadAsStringAsync();
                    var Items = JsonConvert.DeserializeObject<ObservableCollection<DeviceDto>>(content);
                    return Items;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            return null;
        }

        internal void ChangeStateOfAirCon()
        {
            //TODO
        }
    }
}