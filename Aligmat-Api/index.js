const express = require('express');
const moceansdk = require('mocean-sdk');

const app = express();

app.use(express.json());

const mocean = new moceansdk.Mocean(
        new moceansdk.Client({apiToken: "apit-XiYnVxFbWC2C0SwlodeHGCRHFYCAgaR4-iFh30"})
);

app.get('/', (req, res) => {
    res.send('ALIGMAT WARNING SYSTEM');
});

app.post('/send-sms', (req, res) => {
    const { numbers } = req.body;

    numbers.forEach(async (number) => {
        // if (number != '639916965106') return console.log(`Skipping number ${number} as it is not the target number.`);
        
        // mocean.sms().send({
        //     'mocean-from': 'ALIGMAT',
        //     'mocean-to': number,
        //     'mocean-text': 'ALIGMAT WARNING SYSTEM: JEZZ ANG IMO TOILET GABAHA!!!'
        // }, function(err, res) {
        //     if(err) throw err;
        //     console.log(res);
        // });
        
        console.log(`Sending SMS to ${number}...`);

        await delay(10000);
    });

    return res.status(200).json({ message: 'SMS sending initiated.' });
});

app.listen(1921);

function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}