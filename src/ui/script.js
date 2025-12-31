async function search() {
    const query = document.getElementById('query').value;
    const container = document.getElementById('results-container');

    container.innerHTML = "<p>Searching...</p>";

    try {
        const response = await fetch(`http://localhost:5000/search?q=${query}`);
        const data = await response.json(); // This is the 'results' list from Python

        container.innerHTML = ""; // Clear the "Searching..." text

        if (data.length === 0) {
            container.innerHTML = "<p>No results found.</p>";
            return;
        }

        // Loop through results and create HTML elements
        data.forEach(result => {
            const resultDiv = document.createElement('div');
            resultDiv.className = 'result-item';
            resultDiv.innerHTML = `
                <small>Document ID: ${result.id}</small><br>
                <a href="${result.url}" target="_blank">${result.url}</a>
            `;
            container.appendChild(resultDiv);
        });

    } catch (error) {
        container.innerHTML = "<p style='color:red;'>Error connecting to backend.</p>";
    }
}