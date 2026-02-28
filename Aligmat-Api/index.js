const express = require('express');
const moceansdk = require('mocean-sdk');

const app = express();

app.use(express.json());

const mocean = new moceansdk.Mocean(
    new moceansdk.Client({
        apiToken: "apit-TUPL1LraxHrOg7Bt0FvQna1CUxhDzUwg-E9PtK"
    })
);

app.get('/', (req, res) => {
    res.send('ALIGMAT WARNING SYSTEM');
});

app.post('/send-sms', async (req, res) => {
    const { numbers, water_level } = req.body;

    numbers.forEach(async (number) => {
        mocean.sms().send({
            'mocean-from': 'MOCEAN',
            'mocean-to': number,
            'mocean-text': `ALIGMAT!: THE WATER LEVEL HAS REACHED ${water_level} CM. PLEASE TAKE NECESSARY PRECAUTIONS AND STAY ALERT. `
        }, function(err, res) {
            if(err) throw err;
            console.log(res);
        });
        
        console.log(`Sending SMS to ${number}...`);
        await delay(10000);
    });

    return res.status(200).json({ message: 'SMS sending initiated.' });
});

app.listen(3000, "0.0.0.0",() => {
    console.log('Server is running on port 3000');
});

function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}