using System;
using System.Collections.ObjectModel;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Ondo.Api.Devices;
using Ondo.WearableApp.Model;
using Org.Json;

namespace Ondo.WearableApp
{
    class WebApiService
    {
        public async Task<ObservableCollection<AirConDto>> RefreshDataAsync()
        {
            var url = "https://ondo-sea.azurewebsites.net/api/aircon";
            try
            {
                HttpClient client = new HttpClient();
                var response = await client.GetAsync(url);

                if (response.IsSuccessStatusCode)
                {
                    var content = await response.Content.ReadAsStringAsync();
                    var Items = JsonConvert.DeserializeObject<ObservableCollection<AirConDto>>(content);
                    return Items;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            return null;
        }

        internal async Task<bool> ChangeStateOfAirCon(AirConDto airConDto)
        {
            var url = "https://ondo-sea.azurewebsites.net/api/aircon";
            try
            {
                var airconJson = JsonConvert.SerializeObject(airConDto);
                var httpcontent = new StringContent(airconJson.ToString(), Encoding.UTF8, "application/json");

                HttpClient client = new HttpClient();
                var response = await client.PostAsync(url, httpcontent);

                if (response.IsSuccessStatusCode)
                {
                    var content = await response.Content.ReadAsStringAsync();
                    var Items = JsonConvert.DeserializeObject<ObservableCollection<DeviceDto>>(content);
                    return true;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            return false;
        }
    }
}