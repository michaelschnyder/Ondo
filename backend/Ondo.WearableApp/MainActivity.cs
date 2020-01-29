
using Android.App;
using Android.Widget;
using Android.OS;
using Android.Support.Wearable.Activity;
using Android.Views;
using Ondo.Api.Devices;
using GalaSoft.MvvmLight.Helpers;

namespace Ondo.WearableApp
{
    [Activity(Label = "@string/app_name", MainLauncher = true)]
    public class MainActivity : WearableActivity
    {
        protected override async void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.activity_main);

            SetAmbientEnabled();

            await Vm.GetData();
            PeopleListView.Adapter = Vm.Devices.GetAdapter(GetPersonView);
        }

        private View GetPersonView(int position, DeviceDto device, View convertView)
        {
            View view = convertView ?? LayoutInflater.Inflate(Resource.Layout.aircons, null);

            var airconItem = view.FindViewById<TextView>(Resource.Id.AirconItem);
            
            airconItem.Text = device.Location;
            airconItem.Tag = device.DeviceId;
            airconItem.SetCommand("Click", Vm.ChangeStateOfAirCon);

            return view;
        }

        private DeviceViewModel Vm => ViewModelLocator.Instance.DeviceViewModel;

        private ListView PeopleListView => FindViewById<ListView>(Resource.Id.AirconList);
    }
}


