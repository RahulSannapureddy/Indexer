async function search() {
    const queryInput = document.getElementById('query');
    const query = queryInput.value.trim();
    const container = document.getElementById('results-container');

    // 1. Validation: Don't search for empty strings
    if (!query) return;

    container.innerHTML = "<p class='status-msg'>Searching...</p>";

    try {
        // 2. Change: Use a relative path so it works on any host/port
        const response = await fetch(`/search?q=${encodeURIComponent(query)}`);

        if (!response.ok) throw new Error('Network response was not ok');

        const data = await response.json();

        container.innerHTML = ""; // Clear the status text

        if (data.length === 0) {
            container.innerHTML = "<p class='status-msg'>No results found.</p>";
            return;
        }

        // 3. Loop through results and create HTML elements
        data.forEach(result => {
            const resultDiv = document.createElement('div');
            resultDiv.className = 'result-item';

            // Note: Using textContent or similar is safer, but keeping your format:
            resultDiv.innerHTML = `
                <small>Document ID: ${result.id}</small><br>
                <a href="${result.url}" target="_blank" rel="noopener noreferrer">${result.url}</a>
            `;
            container.appendChild(resultDiv);
        });

    } catch (error) {
        console.error("Search Error:", error);
        container.innerHTML = "<p style='color:red;'>Error connecting to backend.</p>";
    }
}

// 4. Added: Allow user to press "Enter" to search
document.getElementById('query').addEventListener('keypress', function (e) {
    if (e.key === 'Enter') {
        search();
    }
});