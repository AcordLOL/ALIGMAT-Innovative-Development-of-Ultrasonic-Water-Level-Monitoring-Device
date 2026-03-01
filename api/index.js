const express = require('express');
const axios = require('axios');
const dotenv = require('dotenv');
const cors = require('cors');

dotenv.config();

const app = express();
app.use(express.json());;
app.use(cors());;

let waterLevel = 0;
const port = 3000;

setInterval(() => {
    waterLevel = Math.max(0, Math.min(100, waterLevel + (Math.random() * 4 - 2)));
}, 2000);

app.get('/', (req, res) => {
    res.send('Welcome to the Aligmat API. Use /water-level to get the current water level.');
});

app.get('/water-level', (req, res) => {
    res.set({
        'Content-Type': 'application/json',
        'Connection': 'keep-alive',
        'Keep-Alive': 'timeout=5, max=1000'
    });

    res.json({
        level: waterLevel.toFixed(2),
        unit: 'cm',
        timestamp: new Date().toISOString()
    });
});

app.post('/water-level', async (req, res) => {
    res.status(200).json({ message: 'Saving water level' }).send();
    const { water_level } = req.body;

    waterLevel = water_level;
});

app.post('/send-sms', async (req, res) => {
    res.status(200).json({ message: 'SMS sending initiated.' }).send()
    
    const { numbers, water_level } = req.body;
    const url = 'https://dashboard.philsms.com/api/v3/sms/send';

    const payload = {
        recipient: `${numbers.join(',')}`,
        sender_id: 'PhilSMS',
        type: "plain",
        message: `ALIGMAT. THE WATER LEVEL HAS REACHED ${water_level} CM. PLEASE TAKE NECESSARY PRECAUTIONS AND STAY ALERT.`
    };

    try {
        const response = await axios.post(url, payload, {
            headers: {
                'Authorization': `Bearer ${process.env.SMS_token}`,
                'Content-Type': 'application/json',
                'Accept': 'application/json'
            }
        });

        console.log('Success:', response.data);
    } catch (error) {
        console.error('Error sending SMS:', error.response ? error.response.data : error.message);
    }
});

app.listen(port, "0.0.0.0",() => {
    console.log(`Server is running on port ${port}`);
});

function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}